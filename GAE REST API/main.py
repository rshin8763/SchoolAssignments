## Author: Ryan Shin
## Date: 11/21/2017 
## This handles routing for my webapp2 backend of my Rest API

from google.appengine.ext import ndb
from google.appengine.api import urlfetch
import webapp2
import os.path
import handlers

# [START app]
allowed_methods = webapp2.WSGIApplication.allowed_methods
new_allowed_methods = allowed_methods.union(('PATCH',))
webapp2.WSGIApplication.allowed_methods = new_allowed_methods

app = webapp2.WSGIApplication([
    webapp2.Route(r'/users', handler = handlers.UserHandler, name = 'user'),
    webapp2.Route(r'/users/<uid:>', handler = handlers.UserHandler, name = 'user'),
    webapp2.Route(r'/users/<uid:>/workouts', handler = handlers.WorkoutListHandler, name = 'workouts'),
    webapp2.Route(r'/users/<uid:>/workouts/<wid:>', handler = handlers.WorkoutHandler, name = 'workout'),
    ], debug=True)
# [END app]

