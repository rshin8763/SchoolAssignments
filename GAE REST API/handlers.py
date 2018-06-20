# Author: Ryan Shin
# Date: 11/2017
# This handles the various URI's of my REST API using the WebApp2 Framework
# in Python. 

from google.appengine.ext import ndb
from google.appengine.api import urlfetch
import json
import webapp2
import os.path
from random import choice
import string
import urllib
import ndb_models

APIKEY = "eab7b8df853fad822a5824a2a52406e5"

def updateSpeed(workout):
    if workout.duration_minutes == 0:
        #undefined flag
        workout.avg_speed = -1
    else:
        workout.avg_speed = workout.distance_miles / workout.duration_minutes * 60

def authorize(access_token):
    try:
        headers = {'Authorization': 'Bearer ' + access_token}
        result = urlfetch.fetch(
                url="https://www.googleapis.com/plus/v1/people/me",
                validate_certificate = True,
                headers=headers
                )
        ##print result.status_code
        if result.status_code == 401:
            return None
        else:
            user_data = json.loads(result.content)
            return user_data['emails'][0]['value']

    except urlfetch.Error:
        logging.exception('Caught exception fetching url')
        return None

def validUser(uid):
    if uid == None:
        return False
    user = ndb_models.User.query(ndb_models.User.id == uid).get()
    if user == None:
        return False
    else:
        return True

def validWorkout(wid, uid):
    if wid == None:
        return False
    ## Make sure workout exists and is owned by User with uid
    workout = ndb_models.Workout.query(ndb_models.Workout.id == wid, ndb_models.Workout.user == uid).get()
    if workout == None:
        return False
    else:
        return True

def userEmailExists(email):
    user = ndb_models.User.query(ndb_models.User.email == email).get()
    if user == None:
	return False
    else:
	return True

def isAuthorized(uid, access_token):
    email = authorize(access_token)
    if email == None:
        return False
    user = ndb_models.User.query(ndb_models.User.id == uid).get()
    if user == None:
        return False
    ## Check if Google + email matches email of user being accessed
    if email == user.email:
        return True
    else:
        #Unauthorized user
        return False

def generateURLQueryfromDict(url, d):
    first = True
    for key in d:
        if (first):
            url += '?'
            first = False
        else:
            url += '&'
        url = url + key + '=' + d[key]
    return url

class UserHandler(webapp2.RequestHandler):
    def post(self):
        ## TODO see if there is a more inutitive way to handle the optional /
        request_data = json.loads(self.request.body)
        ## Check for all request parameters
        if ("access_token" in request_data) and ("fname" in request_data) and ("lname" in request_data):
            email = authorize(request_data["access_token"])
            if email == None:
		self.response.status = 403
		self.response.write("Access token invalid!")
                return
	    if userEmailExists(email):
		self.response.status = 403
		self.response.write("User email in use!")
	    else:
	    #If user with that email doesnt exist, create user
		new_user = ndb_models.User(
			fname = request_data['fname'],
			lname = request_data['lname'],
			email = email
			)
		new_user.put()
		new_user.id = new_user.key.urlsafe()
		new_user.self = '/users/' + new_user.id
		new_user.put()
		user_dict = new_user.to_dict()
		self.response.status = 201
		self.response.write(json.dumps(user_dict))
        else:
            self.response.status = 400
            self.response.write("Need fname, lname, and access_token")

    def patch(self, uid):
        request_data = json.loads(self.request.body)
        if validUser(uid):
            if isAuthorized(uid, request_data["access_token"]):
                user = ndb.Key(urlsafe=uid).get()
                if ('fname' in request_data):
                    user.fname = request_data['fname']
                if ('lname' in request_data):
                    user.lname = request_data['lname']
                user.put()
                user_dict = user.to_dict()
                self.response.status = 200
                self.response.write(json.dumps(user_dict))
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("Invalid User")

    def get(self, uid):
        access_token = self.request.headers["access_token"]
        if validUser(uid):
            if isAuthorized(uid, access_token):
		user = ndb.Key(urlsafe=uid).get()
		self.response.write(json.dumps(user.to_dict()))
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
	    self.response.status = 404
            self.response.write("Invalid User")

    def delete(self, uid):
        access_token = self.request.headers["access_token"]
        if validUser(uid):
	    if isAuthorized(uid, access_token):
		#TODO delete all workouts from user
                workouts = ndb_models.Workout.query(ndb_models.Workout.user == uid)
                for workout in workouts:
                    workout.key.delete()

		#Delete Entity
		ndb.Key(urlsafe=uid).delete()
		self.response.status = 204
		self.response.write("User Deleted")
	    else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("Invalid User")

class WorkoutHandler(webapp2.RequestHandler):
    def get(self, uid, wid):
        access_token = self.request.headers["access_token"]
        if validUser(uid):
            if isAuthorized(uid, access_token):
                if validWorkout(wid, uid):
                    workout = ndb.Key(urlsafe=wid).get()
                    self.response.write(json.dumps(workout.to_dict()))
                else:
                    self.response.status = 404
                    self.response.write("Workout not found")
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("User not found")

    def patch(self, uid, wid):
        request_data = json.loads(self.request.body)
        if validUser(uid):
            if isAuthorized(uid, request_data["access_token"]):
                if validWorkout(wid, uid):
                    workout = ndb.Key(urlsafe=wid).get()
                    if ("weather" in request_data):
                        workout.weather = request_data["weather"]
                    if ("zipcode" in request_data):
                        workout.zipcode = int(request_data["zipcode"])
                    if ("distance_miles" in request_data):
                        workout.distance_miles = float(request_data["distance_miles"])
                    if ("duration_minutes" in request_data):
                        workout.duration_minutes = float(request_data["duration_minutes"])
                    updateSpeed(workout)
                    workout.put()
                    self.response.status = 200
                    # Send edited workout
                    self.response.write(json.dumps(workout.to_dict()))

                else:
                    self.response.status = 404
                    self.response.write("Workout not found")
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("User not found")

    def delete(self, uid, wid):
        access_token = self.request.headers["access_token"]
        if validUser(uid):
            if isAuthorized(uid, access_token):
                if validWorkout(wid, uid):
                    ## Assumption: wid are all unique. If later changed to integers only unique per user,this will break
                    workout = ndb.Key(urlsafe=wid).delete()
                    self.response.status = 204
                    self.response.write("Workout Deleted")
                else:
                    self.response.status = 404
                    self.response.write("Workout not found")
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("User not found")

class WorkoutListHandler(webapp2.RequestHandler):
    ## Get uid list
    def get(self, uid):
        access_token = self.request.headers["access_token"]
        if validUser(uid):
            if isAuthorized(uid, access_token):
                workouts = ndb_models.Workout.query(ndb_models.Workout.user == uid)
                workoutArray =[]
                for workout in workouts:
                    workoutArray.append(workout.to_dict())
                self.response.write(json.dumps(workoutArray))
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("User not found")

    def delete(self, uid):
        access_token = self.request.headers["access_token"]
        if validUser(uid):
            if isAuthorized(uid, access_token):
                workouts = ndb_models.Workout.query(ndb_models.Workout.user == uid)
                workoutArray =[]
                for workout in workouts:
                    workout.key.delete()
                self.response.status = 204
                self.response.write("Workouts Deleted")
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("User not found")

    def post(self, uid):
        request_data = json.loads(self.request.body)
        ## Check for all request parameters
        if validUser(uid):
            if isAuthorized(uid, request_data["access_token"]):
                if ("zipcode" in request_data) and ("distance_miles" in request_data) and ("duration_minutes" in request_data):
                    # TODO input validation
		    new_workout = ndb_models.Workout(
                            distance_miles = float(request_data["distance_miles"]),
                            duration_minutes = float(request_data["duration_minutes"]),
                            zipcode = int(request_data["zipcode"])
		    )
                    new_workout.user = uid
                    new_workout.put()
                    new_workout.id = new_workout.key.urlsafe()
                    new_workout.self = '/users/' + uid + '/workouts/' + new_workout.id

                    # Gets weather data from API
                    url = "https://api.openweathermap.org/data/2.5/weather?APPID="+APIKEY+"&zip="+str(new_workout.zipcode)+",us"
                    result = urlfetch.fetch(url)
                    if result.status_code == 200:
                        weather_data = json.loads(result.content)
                        print weather_data
                        print weather_data["weather"][0]["main"]
                        new_workout.weather =  weather_data["weather"][0]["main"]
                    else:
                        new_workout.weather = "Unavailable"

                    # Calculates avg_speed
                    updateSpeed(new_workout)
                    new_workout.put()

                    workout_dict = new_workout.to_dict()
                    self.response.status = 201
                    self.response.write(json.dumps(workout_dict))

                else:
                    self.response.status = 400
                    self.response.write("Need zipcode, distance_miles, and duration_minutes")
            else:
                self.response.status = 401
                self.response.write("Unauthorized access to resource")
        else:
            self.response.status = 404
            self.response.write("User not found")
