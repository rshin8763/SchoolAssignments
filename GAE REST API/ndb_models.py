from google.appengine.ext import ndb

class User(ndb.Model):
    id = ndb.StringProperty()
    fname = ndb.StringProperty(required=True)
    lname = ndb.StringProperty(required=True)
    email = ndb.StringProperty(required=True)
    self = ndb.StringProperty()

class Workout(ndb.Model):
    id = ndb.StringProperty()
    weather = ndb.StringProperty()
    distance_miles = ndb.FloatProperty()
    duration_minutes = ndb.FloatProperty()
    avg_speed = ndb.FloatProperty()
    zipcode = ndb.IntegerProperty()
    user = ndb.StringProperty(required=True)
    self = ndb.StringProperty()
