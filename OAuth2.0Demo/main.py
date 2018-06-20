#########################################################
# Ryan Shin
# CS496 Assignment 3:
# OAuth2.0 Implementation
# 2/11/18
#########################################################

from google.appengine.ext import ndb
from random import choice
from google.appengine.api import urlfetch
import string
import json
import webapp2
import urllib

state = ''.join(choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(10))
client_secret = 'tHwJbdENJ2NR8CflcwH8K3J-'
client_id = '184309771068-h6n4vqbtmtklnahs8v7ur2ogaflr8l5o.apps.googleusercontent.com'
redirect_uri = 'http://oauth2-demo-194922.appspot.com/callback'

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

class OAuthHandler(webapp2.RequestHandler):
    def get(self):
        access_code = self.request.get('code')
        print access_code
        stateVal = self.request.get('state')
        if stateVal != state:
            self.response.write('State value does not match!')
            return

        url = "https://www.googleapis.com/oauth2/v4/token"
        # Generate request
        try:
            headers = {'Content-Type': 'application/x-www-form-urlencoded'}
            form_data = urllib.urlencode({
                'code': access_code,
                'client_id': client_id,
                'client_secret': client_secret,
                'redirect_uri':redirect_uri,
                'grant_type':'authorization_code'
                })

            print "making request"
            result = urlfetch.fetch(
                url= "https://www.googleapis.com/oauth2/v4/token",
                payload=form_data,
                method=urlfetch.POST,
                headers=headers,
                validate_certificate = True
                )
            json_data = json.loads(result.content)

            print json_data

            headers = {'Authorization': 'Bearer ' + json_data['access_token']}
            result = urlfetch.fetch(
                    url="https://www.googleapis.com/plus/v1/people/me",
                    validate_certificate = True,
                    headers=headers
                    )
            user_data = json.loads(result.content)

            self.response.write('<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"><script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script><script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>')

            email = user_data['emails'][0]['value']
            name = user_data['name']['givenName'] + ' ' + user_data['name']['familyName']

            self.response.write("<html><div class='container'><div class='jumbotron'><p><h1>OAuth2.0 Demo</h1></p><div><p><h3>Your Name: "+name+"</h3><h3>Your E-mail: "+email+"</h3></p></div></div></div></html>")
            
        except urlfetch.Error:
            logging.exception('Caught exception fetching url')

# [START main_page]
class MainPage(webapp2.RequestHandler):
    def get(self):
        # Redirect to Google for Authentication
         
        url = 'http://accounts.google.com/o/oauth2/v2/auth'
        d = {}
        d['response_type'] = 'code'
	#d['client_secret'] = 'tHwJbdENJ2NR8CflcwH8K3J-'
        d['client_id'] = client_id
        d['redirect_uri'] = redirect_uri
        d['scope'] = 'email'
        ## Generate random state (example adapted from stackoverflow.com/questions/2257441/)
        d['state'] = state 

        url = generateURLQueryfromDict(url, d)

        self.response.write('<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"><script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script><script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>')

        self.response.write("<html><div class= 'container'><div class='jumbotron'><p><h1>OAuth2.0 Demo</h1></p><p>This page is a demonstration of OAuth 2.0 Server-side flow.</br> Please click the link to be authenticated through Google.</p><p><a href=" +url +"> Click Here </a></p></div></div></html>")

# [END main_page]

# [START app]
allowed_methods = webapp2.WSGIApplication.allowed_methods
new_allowed_methods = allowed_methods.union(('PATCH',))
webapp2.WSGIApplication.allowed_methods = new_allowed_methods
app = webapp2.WSGIApplication([
    ('/', MainPage),
    ('/callback', OAuthHandler)
    ], debug=True)
# [END app]
