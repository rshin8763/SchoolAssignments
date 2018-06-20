from google.appengine.ext import ndb
import json
import webapp2
import os.path

## Returns True if 'boat_id' is a valid Boat in the NDB, 
## else returns False
def validBoat(boat_id):
    if boat_id == None:
        return False
    boat = Boat.query(Boat.id == boat_id).get()
    if boat == None:
        return False
    else:
        return True

## Returns True if 'slip_id' is a valid Slip in the NDB, 
## else returns False.
def validSlip(slip_id):
    if slip_id == None:
        return False
    slip = Slip.query(Slip.id == slip_id).get()
    if slip == None:
        return False
    else:
        return True

def slipNumberExists(number):
    slip = Slip.query(Slip.number == number).get()
    if slip == None:
        return False
    else:
        return True

## Sets boat.at_sea to True and clears paired boat from slip indentified
## by slip_id. If slip_id is null, does nothing.
def clearSlip(slip_id):
    if slip_id == None:
        return
    else:
        slip = ndb.Key(urlsafe = slip_id).get()
        if slip.current_boat != None:
            boat = ndb.Key(urlsafe = slip.current_boat).get()
            boat.at_sea = True
            boat.put()
        slip.current_boat = None
        slip.boatLink = None
        slip.put()

## Gets the slip id of the slip that the boat, boat_id, is currently in.
## Returns null if there is no paired slip.
def getSlipIdFromBoat(boat_id):
    slip = Slip.query(Slip.current_boat == boat_id).get()
    if slip == None:
        return None
    else:
        return slip.id

class Boat(ndb.Model):
    id = ndb.StringProperty()
    name = ndb.StringProperty(required=True)
    type = ndb.StringProperty(required=True)
    length = ndb.IntegerProperty(required=True)
    at_sea = ndb.BooleanProperty(default=True)
    self = ndb.StringProperty()

class Slip(ndb.Model):
    id = ndb.StringProperty()
    number = ndb.IntegerProperty(required=True)
    current_boat = ndb.StringProperty()
    arrival_date = ndb.StringProperty()
    self = ndb.StringProperty()
    boatLink = ndb.StringProperty()

class BoatHandler(webapp2.RequestHandler):
    def post(self):
        boat_data = json.loads(self.request.body)
        if ("name" in boat_data) and ("type" in boat_data) and ("length" in boat_data):
            new_boat = Boat( 
                    name=boat_data['name'], 
                    type = boat_data['type'] 
                    )
            try:
                new_boat.length = int(boat_data['length'])
            except ValueError:
                self.response.status = 400
                self.response.write("Invalid value!")
                return

            new_boat.put()
            new_boat.id = new_boat.key.urlsafe()
            new_boat.self = '/boat/' + new_boat.id 
            new_boat.put()
            boat_dict = new_boat.to_dict()
            self.response.status = 201
            self.response.write(json.dumps(boat_dict))
        else:
            self.response.status = 400
            self.response.write("Requires name, type, and length fields")

    def patch(self, id=None):
        boat_data = json.loads(self.request.body)
        if validBoat(id):
            boat = ndb.Key(urlsafe=id).get()
            if ("name" in boat_data):
                boat.name = boat_data['name']
            if ("type" in boat_data):
                boat.type = boat_data['type']
            if ("length" in boat_data):
                try:
                    boat.length = int(boat_data['length'])
                except ValueError:
                    self.response.status = 400
                    self.response.write("Invalid length value!")
                    return
            if ("at_sea" in boat_data):
                if boat_data['at_sea'].lower() == 'false':
                    if boat.at_sea == True:
                        self.response.status = 403
                        self.response.write("Unable to change at_sea status to False")
                        return
                elif boat_data['at_sea'].lower() == 'true':
                    ## Unpair from Slip
                    clearSlip(getSlipIdFromBoat(id))
                    boat.at_sea = True
                else:
                    self.response.status = 400
                    self.response.write("Invalid at_sea value!")
                    return

            boat.put()
            boat_dict = boat.to_dict()
            self.response.write(json.dumps(boat_dict))
        else:
            self.response.status = 400
            self.response.write("Invalid Boat")

    def get(self, id=None):
        if validBoat(id):
            boat = ndb.Key(urlsafe=id).get()
            self.response.write(json.dumps(boat.to_dict()))
        else:
            if id  == None:
                ##get all boats
                q = Boat.query()
                boats = q.fetch(30)
    
                list = []
    
                for b in boats:
                    boat_dict = b.to_dict()
                    list.append(boat_dict)
    
                self.response.write(json.dumps(list))

            else:
                self.response.status = 404
    

    def delete(self, id=None):
        if validBoat(id):
            #Remove boat from slip
            clearSlip(getSlipIdFromBoat(id))

            #Delete Entity
            ndb.Key(urlsafe=id).delete()
            self.response.status = 204
            self.response.write("Entity Deleted")
        else:
            self.response.status = 400
            self.response.write("Invalid Boat")

class SlipHandler(webapp2.RequestHandler):
    def post(self):
        slip_data = json.loads(self.request.body)

        if ("number" in slip_data):
            try:
                int(slip_data['number'])
            except ValueError:
                self.response.status = 400
                self.response.write("Invalid number value!")
                return

            if slipNumberExists(int(slip_data['number'])):
                self.response.status = 403
                self.response.write("Slip with that number already exists!")
                return
            else:
                new_slip = Slip(
                        number = int(slip_data['number'])
                        )
                new_slip.current_boat = None
                new_slip.put()
                new_slip.id = new_slip.key.urlsafe()
                new_slip.self = '/slip/' + new_slip.id 
                new_slip.put()
                slip_dict = new_slip.to_dict()
                self.response.status = 201
                self.response.write(json.dumps(slip_dict))
        else:
            self.response.status = 400
            self.response.write("Creating slip requires 'number' field")

    def patch(self, id=None):
        slip_data = json.loads(self.request.body)
        if validSlip(id):
            slip = ndb.Key(urlsafe=id).get()
            if ("number" in slip_data):
                try:
                    int(slip_data['number'])
                except ValueError:
                    self.response.status = 400
                    self.response.write("Invalid number value!")
                    return
                if int(slip_data['number']) == slip.number:
                    pass
                elif slipNumberExists(int(slip_data['number'])):
                    self.response.status = 403
                    self.response.write("Slip with that number already exists!")
                    return
                else:
                    slip.number = int(slip_data['number'])

            if ("current_boat" in slip_data):
                self.response.status = 403
                self.response.write("Cannot change current_boat at this resource!")
                return
            if ("arrival_date" in slip_data):
                slip.arrival_date = slip_data['arrival_date']
            slip.put()
            slip_dict = slip.to_dict()
            self.response.write(json.dumps(slip_dict))
        else:
            self.response.status = 404

    def get(self, id=None):
        if validSlip(id):
            slip = ndb.Key(urlsafe=id).get()
            self.response.write(json.dumps(slip.to_dict()))
        else:
            if id == None:
                ##get all slips
                q = Slip.query()
                slips = q.fetch(30)

                list = []

                for b in slips:
                    slip_dict = b.to_dict()
                    list.append(slip_dict)

                self.response.write(json.dumps(list))

            else:
                self.response.status = 404


    def delete(self, id = None):
        if validSlip(id):
            #Set paired boat to at sea
            slipKey = ndb.Key(urlsafe=id)
            clearSlip(id)

            #Delete Slip
            slipKey.delete()
            self.response.status = 204
            self.response.write("Slip Deleted")
        else:
            self.response.status = 404
            self.response.write("Invalid Slip")

class BoatInSlipHandler(webapp2.RequestHandler):
    def put(self, id = None):
        if validSlip(id):
            slip = ndb.Key(urlsafe=id).get()
            ## If slip is vacant
            if slip.current_boat == None:
                boat_data = json.loads(self.request.body)
                if ("boat_id" in boat_data and "arrival_date" in boat_data):
                    ## Valid boat_id
                    bid = boat_data['boat_id']
                    if validBoat(bid):
                        ## Update relationships
                        boat = ndb.Key(urlsafe=bid).get()

                        ## If boat is docked, change Slips
                        clearSlip(getSlipIdFromBoat(id))
                        boat.at_sea = False
                        slip.current_boat = bid
                        slip.boatLink = '/boat/' + bid

                        boat.put()
                        slip.put()

                        slip_dict = slip.to_dict()
                        self.response.status = 200
                        self.response.write(json.dumps(slip_dict))
                    ## Invalid boat_id
                    else:
                        self.response.status = 404
                        self.response.write("Invalid Boat ID")
                ## No boat_id or arrival date supplied
                else:
                    self.response.status = 400
                    self.response.write("Missing fields in request.")
            ## If slip is occupied
            else:
                self.response.status = 403
                self.response.write("Slip is occupied")
        ## Invalid slip
        else:
            self.response.status = 404
            self.response.write("Invalid Slip ID")

    def get(self, id=None):
        print id
        if validSlip(id):
            slip = ndb.Key(urlsafe=id).get()
            if slip.current_boat == None:
                self.response.status = 404
            else:
                boat = ndb.Key(urlsafe=slip.current_boat).get()
                self.response.write(json.dumps(boat.to_dict()))
    
# [START main_page]
class MainPage(webapp2.RequestHandler):
    def get(self):
        self.response.write("Assignment 2.\nConsult URI Resource")
# [END main_page]

# [START app]
allowed_methods = webapp2.WSGIApplication.allowed_methods
new_allowed_methods = allowed_methods.union(('PATCH',))
webapp2.WSGIApplication.allowed_methods = new_allowed_methods
app = webapp2.WSGIApplication([
    ('/', MainPage),
    ('/boat', BoatHandler),
    ('/boat/(.*)', BoatHandler),
    ('/slip', SlipHandler),
    ('/slip/(.*)/current_boat', BoatInSlipHandler),
    ('/slip/(.*)', SlipHandler)
    ], debug=True)
# [END app]
