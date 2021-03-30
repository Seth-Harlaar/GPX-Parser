'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// define gpx library
var gpx = ffi.Library( './libgpxparser', {
  'gpxFileToJSON': ['string', [ 'string' ]],
  'gpxRoutesToJSON': ['string', [ 'string' ]],
  'gpxTracksToJSON': ['string', [ 'string' ]]
});



// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});

// send back a JSON obj containing all the gpx file data
app.get('/retrieveFiles', function(req, res){

  console.log('Request to retrieve files on server');

  // read the name of every file the upload dir
  fs.readdir(path.join(__dirname+'/uploads/'), function( err, files) {

    if(err == null) {
      // start with empty JSON
      var gpxFilesObject = {
      }

      // for each file, parse it and add it to the JSON obj
      files.forEach(file => {
        // check first that file's extension is gpx
        // solution to get extension found here: https://stackoverflow.com/questions/190852/how-can-i-get-file-extensions-with-javascript/1203361#1203361
        var fileExtension = file.substring(file.lastIndexOf('.') + 1, file.length) || file;

        if( fileExtension == 'gpx' ){
          // get the JSON string from c code and parse it to json
          var gpxObject = JSON.parse(gpx.gpxFileToJSON( path.join( __dirname + '/uploads/' + file) ));
          
          // add it to the object
          gpxFilesObject[file] = gpxObject;
        }
      });

      // send it back
      res.send(  {
          gpxFilesObject: gpxFilesObject
        }
      );
    // error  
    } else {
      console.log('Error in retrieving file names')
    }
  });
});

// get the routes that exist in a given gpx file
app.get('/getRoutes', function( req, res) {
  let newPath = path.join( __dirname + '/uploads/' + req.query.name );

  // see if the file exists
  fs.access( newPath, fs.F_OK, (err) => {
    if( !err ) {
      // get the JSON string that represents all the routes
      var gpxRoutesObject = JSON.parse( gpx.gpxRoutesToJSON( path.join( __dirname + '/uploads/' + req.query.name )));
      res.send( {
        gpxRoutesObject: gpxRoutesObject
      });
    } else {
      console.log( err );
    }
  })
});

// get the tracks that exist in a given gpx file
app.get('/getTracks', function( req, res) {
  let newPath = path.join( __dirname + '/uploads/' + req.query.name );

  // see if the file exists
  fs.access( newPath, fs.F_OK, (err) => {
    if( !err ) {
      // get the JSON string that represents all the tracks
      var gpxTracksObject = JSON.parse( gpx.gpxTracksToJSON( path.join( __dirname + '/uploads/' + req.query.name )));
      res.send( {
        gpxTracksObject: gpxTracksObject
      });
    } else {
      console.log( err );
    }
  })
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


app.get('/getOtherData', function( req, res) {

  var componentName = req.query.name;
  var fileName = req.query.fileName;
  var route = ( req.query.route == 'true');


  if( route ){
    // make the c function call to get the otherData JSON string for routes
    console.log('getting other data for route named: ' + componentName + ' from: ' + fileName);
  } else {
    // make the c function call to get the otherData JSON string for tracks
    console.log('getting other data for track named: ' + componentName + ' from: ' + fileName);

  }
});