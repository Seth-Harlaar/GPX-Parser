'use strict'

// C library API
const ffi = require('ffi-napi');

// test
// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');
const mysql = require('mysql2/promise');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { NONAME } = require('dns');
const { stringify } = require('querystring');
const { connect } = require('http2');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// define gpx library
var gpx = ffi.Library( './libgpxparser', {
  'gpxFileToJSON': ['string', [ 'string' ]],
  'gpxRoutesToJSON': ['string', [ 'string' ]],
  'gpxTracksToJSON': ['string', [ 'string' ]],
  'getOtherDataJSON': ['string', ['string', 'int', 'string']],
  'makeNewDoc': ['string', ['string', 'string']],
  'addNewRoute': ['string', ['string', 'string']],
  'addWptToRoute' : ['string', ['string', 'string', 'string']],
  'renameRoute' : ['string', ['string', 'string', 'string']],
  'renameTrack' : ['string', ['string', 'string', 'string']],
  'routesBetweenToJSON': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  'tracksBetweenToJSON': ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  'validateGPXFile' : ['string', ['string']],
  'routeNameToWptJSON' : ['string', ['string', 'string']]
});


let globalConnection;
let globalUser;
let globalPass;
let globalName;
let globalConnected = false;

// Send HTML at root, do not change
app.get('/', function(req,res){
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
  var fileName = uploadFile.name;

  let add = true;

  // get the file name, check all the names in the dir and see if there is one already in there
  fs.readdir(path.join(__dirname+'/uploads/'), function( err, files) {
    files.forEach( file => {
      if( fileName == file ){
        add = false;
      }
    });
  
    if( add == false ){
      console.log('Not adding file: ' + fileName + ' because one with the same name already exists on the server' );
      res.redirect('/');
    } else {
      // Use the mv() method to place the file somewhere on your server
      uploadFile.mv('uploads/' + uploadFile.name, function(err) {
        if(err) {
          return res.status(500).send(err);
        }
    
        res.redirect('/');
      });
    }
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
      var count = 0;

      // start with empty JSON
      var gpxFilesObject = {
      }

      // for each file, parse it and add it to the JSON obj
      files.forEach(file => {

        // check first that file's extension is gpx
        // solution to get extension found here: https://stackoverflow.com/questions/190852/how-can-i-get-file-extensions-with-javascript/1203361#1203361
        var fileExtension = file.substring(file.lastIndexOf('.') + 1, file.length) || file;

        if( fileExtension == 'gpx' ){

          // check if its valid first
          // make the path
          var newPath = path.join( __dirname + '/uploads/' + file );

          // validate by passing to c function
          var valid = gpx.validateGPXFile( newPath );

          console.log(file + ' was valid: ' + valid );
          // onyl if its valid
          if( valid == 'true' ){
            // get the JSON string from c code and parse it to json
            var gpxObject = JSON.parse(gpx.gpxFileToJSON( path.join( __dirname + '/uploads/' + file) ));
            
            // add it to the object
            gpxFilesObject[file] = gpxObject;
            // increment the length
            count++;
          }
        }
      });

      // send it back
      res.send(  {
          count: count,
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

  let newPath = path.join( __dirname + '/uploads/' + fileName );


  if( route ){
    // make the c function call to get the otherData JSON string for routes
    console.log('getting other data for route named: ' + componentName + ' from: ' + fileName);

    var otherData = gpx.getOtherDataJSON( newPath, 1, componentName );
  } else {
    // make the c function call to get the otherData JSON string for tracks
    console.log('getting other data for track named: ' + componentName + ' from: ' + fileName);
    var otherData = gpx.getOtherDataJSON( newPath, 0, componentName );

  }

  // take out all newlines because they cause errros
  // method found here: https://stackoverflow.com/questions/10805125/how-to-remove-all-line-breaks-from-a-string
  otherData = otherData.replace(/(\r\n|\n|\r)/gm, "");
  console.log( otherData );

  var sendData;

  if( otherData == 'error' ){
    res.send({
      success: false,
      content: 'none'
    });
  } else {
    sendData = JSON.parse( otherData );
    res.send({
      success: true,
      content: sendData
    });
  }
});

app.get('/getPoints', function( req, res){
  // get info
  var fileName = req.query.fileName;
  var routeName = req.query.routeName;

  // send to c file
  var data = JSON.parse( gpx.routeNameToWptJSON( path.join( __dirname + '/uploads/' + fileName ), routeName ) );

  res.send({
    success: true,
    gpxPointData: data
  });
});


app.get('/newDoc', function(req, res){
  console.log('post request to add new file');
  var fileName = req.query.fileName;
  var creator = req.query.creator;
  var version = parseFloat( req.query.version );
  
  var same = false;

  // check unique name
  fs.readdir(path.join(__dirname+'/uploads/'), function( err, files) {
    if( err == null ){
      
      files.forEach(file => {
        // if its the same as one of the other files
        if( file == fileName ){
          console.log('Cannot create another new file with same name as file already on server');
          same = true;
        }
      });

      if( same == false ){
        // if its unique - call the c function to save file
        var docData = {
          "version":version,
          "creator":creator
        };
  
        var result = gpx.makeNewDoc( JSON.stringify(docData), path.join( __dirname + '/uploads/' + fileName ));
  
  
        res.send({
          success: result
        });
        // validate file
      }
    }
  });
});


app.get('/newRoute', function(req, res){

  var newRouteName = req.query.wptData.newRouteName;
  var fileName = req.query.fileName;

  console.log('making new route with name: ' + newRouteName + ' in file: ' + fileName );

  if( newRouteName == 'Select a File' ){
    res.send({
      select: false
    })
  }

  // make the c function call to add a new route
  var routeData = {
    "name":newRouteName
  }

  var success = gpx.addNewRoute( JSON.stringify(routeData), path.join( __dirname + '/uploads/' + fileName ) );


  // now add the new other data to the new route
  var wpts = req.query.wptData.wpts;

  // for each data
  for( let wpt in wpts){
    // create a json obj with the data
    var wptData = {
      "lat":parseFloat(wpts[wpt].lat), 
      "lon":parseFloat(wpts[wpt].lon)
    }
    
    console.log('adding wpt to route: ' + newRouteName );
    console.log( JSON.stringify(wptData) );
    
    // pass the json, and route and file to c wrapper function
    success = gpx.addWptToRoute( JSON.stringify(wptData), path.join( __dirname + '/uploads/' + fileName ), newRouteName );

  }
  console.log( success );

  res.send({
    select: true,
    success: success
  })
});


app.get('/renameRoute', async function(req, res){

  let rows;
  let fields;

  // need old track name and new track name and file name
  var oldName = req.query.oldName;
  var newName = req.query.newName;
  var fileName = req.query.fileName;

  
  // send to c function
  var success = gpx.renameRoute( oldName, path.join( __dirname + '/uploads/' + fileName ), newName );
  
  if( success == 'true' ){
    console.log('Changed name from ' + oldName + ' to ' + newName + ' in file ' +fileName );
  } else {
    console.log('Failed to change name' );
  }

  // update name in the db
  // get gpx_id
  var findID = "SELECT gpx_id FROM FILE " +
  "WHERE file_name = '" + fileName + "'";

  if( globalConnected ){
    try{
      [rows, fields] = await globalConnection.execute(findID);

      var gpx_id = rows[0]['gpx_id'];

      // update where name and gpx id
      var updateRoute = "UPDATE ROUTE" +
      " SET route_name = '" + newName + "'" +
      " WHERE gpx_id = " + gpx_id + " AND route_name = '" + oldName + "';";

      try{
        console.log( updateRoute );
        [rows, fields] = await globalConnection.execute(updateRoute);

      } catch(e){
        console.log('Error updating route to new name ' + e );
      }
    } catch(e){
      console.log('Error retrieving gpx_id in rename route ' + e );
    }
  }

  res.send({
    success: success
  })
});


app.get('/renameTrack', function(req, res){

  // need old track name and new track name and file name
  var oldName = req.query.oldName;
  var newName = req.query.newName;
  var fileName = req.query.fileName;

  console.log('Change name from ' + oldName + ' to ' + newName + ' in file ' +fileName );

  // send to c function
  var success = gpx.renameTrack( oldName, path.join( __dirname + '/uploads/' + fileName ), newName );
  console.log( success );
  res.send({
    success: success
  })
});


app.get('/pathsBetweenRoutes', function(req, res){

  // need wpts and tolerance
  var lon1 = req.query.lon1;
  var lat1 = req.query.lat1;
  var lon2 = req.query.lon2;
  var lat2 = req.query.lat2;
  var tol  = req.query.tol;

  //console.log(req.query);

  //console.log('searching for routes between: ' + lat1 + '/' + lon1 + ' and ' + lat2 + '/' + lon2 );

  var newPath = path.join( __dirname + '/uploads/' + req.query.fileName );

  // find the routes 
  var routesJSON = JSON.parse( gpx.routesBetweenToJSON( newPath, lat1, lon1, lat2, lon2, tol ) );

  //console.log( routesJSON );

  if( routesJSON.length == 0 ){
    res.send({
      success: true,
      empty: true,
      routeData: routesJSON
    })
  
  } else {
    res.send({
      success: true,
      empty: false,
      routeData: routesJSON
    })
  }
});

app.get('/pathsBetweenTracks', function(req, res){
  // get the info
  // need wpts and tolerance
  var lon1 = req.query.lon1;
  var lat1 = req.query.lat1;
  var lon2 = req.query.lon2;
  var lat2 = req.query.lat2;
  var tol  = req.query.tol;

  console.log( req.query );

  console.log('searching for tracks between: ' + lat1 + '/' + lon1 + ' and ' + lat2 + '/' + lon2 );
  
  // get the path 
  var newPath = path.join( __dirname + '/uploads/' + req.query.fileName );

  // find the tracks 
  var tracksJSON = JSON.parse( gpx.tracksBetweenToJSON( newPath, lat1, lon1, lat2, lon2, tol ) );
  
  if( tracksJSON.length == 0 ){
    res.send({
      success: true,
      empty: true,
      trackData: tracksJSON
    })
  
  } else {
    res.send({
      success: true,
      empty: false,
      trackData: tracksJSON
    })
  }
});



app.get('/validateFiles', function(req, res){

  // need file name,
  var fileName = req.query.fileName;

  // make the path
  var newPath = path.join( __dirname + '/uploads/' + fileName );

  // validate by passing to c function
  var valid = gpx.validateGPXFile( newPath );

  console.log(fileName + ' was valid: ' + valid );

  // return whether file is valid gpx or not
  res.send({
    success: true,
    valid: valid
  });

});



// **************** A4 funcitonality **********************

// create connection
app.get('/login', async function( req, res){
  // get info
  globalUser = req.query.userName;
  globalPass = req.query.password;
  globalName = req.query.dbName;

  // try connecting
  try{
    globalConnection = await mysql.createConnection({
      host: 'dursley.socs.uoguelph.ca',
      user: globalUser,
      password: globalPass,
      database: globalName
    })

    globalConnected = true;
    console.log('successfully connected to the db');
  } catch(e) {
    console.log( 'connection error' + e );
  } finally {
    // send response
    res.send({
      success: globalConnected
    })
  }
});


app.get('/logout', async function(req, res){
  try{
    await globalConnection.end();
  } catch(e){
    console.log('error loggint out of db '+ e );
  }
  globalConnected = false;
  res.send({
    success: true
  })
})


// setup tables 
app.get('/createTables', async function(req, res){
  var success = true;

  let fileTable = "CREATE TABLE IF NOT EXISTS FILE(   " +
  "gpx_id      INT           NOT NULL AUTO_INCREMENT, " + 
  "file_name   VARCHAR(60)   NOT NULL,                " + 
  "ver         DECIMAL(2,1)  NOT NULL,                " + 
  "creator     VARCHAR(60)   NOT NULL,                " + 
  "PRIMARY KEY (gpx_id)                               " + 
  ")";

  let routeTable = "CREATE TABLE IF NOT EXISTS ROUTE(    " +
  "route_id     INT           NOT NULL AUTO_INCREMENT,   " +
  "route_name   VARCHAR(256),                            " +
  "route_len    FLOAT(15,7)   NOT NULL,                  " +
  "gpx_id       INT           NOT NULL,                  " +
  "PRIMARY KEY  (route_id),                              " +
  "FOREIGN KEY  (gpx_id)      REFERENCES FILE(gpx_id) ON DELETE CASCADE   " +
  ")";

  let pointTable = "CREATE TABLE IF NOT EXISTS POINT(     " +
  "point_id     INT             NOT NULL AUTO_INCREMENT,  " +
  "point_index  INT             NOT NULL,                 " +
  "latitude     DECIMAL(11,7)   NOT NULL,                 " +
  "longitude    DECIMAL(11,7)   NOT NULL,                 " +
  "point_name   VARCHAR(256),                             " +
  "route_id     INT             NOT NULL,                 " +
  "PRIMARY KEY  (point_id),                               " +
  "FOREIGN KEY  (route_id)      REFERENCES ROUTE(route_id) ON DELETE CASCADE   " +
  ")";

  if( globalConnected ){
    try{
      let [rows, fields] = await globalConnection.execute( fileTable );
      [rows, fields] = await globalConnection.execute( routeTable );
      [rows, fields] = await globalConnection.execute( pointTable );
      
    } catch(e){
      console.log('Error creating tables ' + e);
      success = false;
    } finally {
      res.send({
        success: success
      })
    }
  }
});


// save a file
app.get('/saveFile', async function(req, res){
  var success = false;
  var unique = true;
  let rows;
  let fields;
  let gpx_id = 0;
  
  // file data 
  var fileName = req.query.fileName;
  var version = req.query.version;
  var creator = req.query.creator;

  // if successfully connected, get the query
  if( globalConnected ){

    // save the file info to FILE table
    var fileInsert = 'INSERT INTO FILE' +
    '(file_name, ver, creator)' +
    'values ( "' + fileName + '",' + version + ', "' + creator +  '")';
  
    // check first that the filename is not already present in the db
    // get list of all file names
    try{
      [rows, fields] = await globalConnection.execute('select file_name from FILE');
      
      // compare against each file name
      for( let row in rows ){
        if( fileName == rows[row].file_name ){
          console.log('File: ' + fileName + ' has already been saved to db')
          unique = false;
        }
      }
    } catch(e){
      console.log('Error retrieving all file names: ' + e);
      unique = false;
    }
  
    // if it is a unique file name and passes try/catch block, send it to the conection
    if( unique ){
      // try insert the file
      try{
        console.log( fileInsert );
        let [rows, fields] = await globalConnection.execute( fileInsert );
        var success = true;
        console.log('successfully saved file: ' + fileName);
      
        gpx_id = rows.insertId;
  
      // if sending the file fails
      } catch(e){
        console.log('Eror trying to insert file ' + e);
        var success = false;    
      }
    }
  
    // at the end of everything
    if( !success ){
      console.log('Could not save files')
    }
  
    res.send({
      connected: globalConnected,
      success: success,
      gpx_id: gpx_id
    });
  }
});



// save a route
app.get('/saveRoute', async function(req, res){
  // get the info - name, length, gpx_id, 
  var name = req.query.name;
  var routeLength = req.query.routeLength;
  var gpx_id = req.query.gpx_id;

  let rows;
  let fields;
  let route_id = 0;

  var success = true;

  if( globalConnected ){
    // save the route
    var routeInsert = "INSERT INTO ROUTE " + 
    "(route_name, route_len, gpx_id) " +
    "values( '" +  name + "', " + routeLength + ", " + gpx_id + ");";
    console.log( routeInsert );
    
    // try execute insert
    try{
      [rows, fields] = await globalConnection.execute( routeInsert );
      route_id = rows.insertId;
  
    } catch(e){
      console.log('Failed inserting route into ROUTE table ' + e);
      success = false;
    }
  }

  res.send({
    connected: globalConnected,
    success: success,
    route_id: route_id
  });

});

app.get('/savePoint', async function(req, res){
  let rows;
  let fields;
  
  var success = true;
  // get info
  var index = req.query.index;
  var lat = req.query.lat;
  var long = req.query.long;
  var pointName = req.query.pointName;
  var route_id = req.query.route_id;

  if( globalConnected ){
    
    // make insert string
    var insertPoint = "INSERT INTO POINT " +
    "(point_index, latitude, longitude, point_name, route_id) " +
    "values (" + index + "," + lat + "," + long + ",'" + pointName + "'," + route_id + ")" 
    
    console.log( insertPoint );
  
    // try execute
    try{
      [rows, fields] = await globalConnection.execute( insertPoint );
    } catch(e){
      console.log('error adding point: ' + e);
      success = false;
    } finally {
      res.send({
        connected: globalConnected,
        success: success
      });
    }
  }
});


app.get('/clearData', async function(req, res){
  var success = true;
  
  // make the query
  var deleteQuery = "DELETE FROM FILE";

  if( globalConnected ){
    try{
      await globalConnection.execute( deleteQuery );
      console.log('Cleared all data');
    } catch(e) {
      console.log('Error while trying to clear all data from db ' + e);
      success = false;
    } finally {
      res.send({
        success: success
      });
    }
  }
});

app.get('/getStatus', async function(req, res){
  var success = true;
  var query;

  let rows;
  let fields;

  // get each piece of info
  if( globalConnected ){
    var files = 0;
    var routes = 0;
    var points = 0;

    // get files
    query = "SELECT COUNT(*) FROM FILE";

    try{
      [rows, fields] = await globalConnection.execute(query);
      files = rows[0]['COUNT(*)'];
    } catch(e){
      console.log('error while getting file count ' + e);
      success = false;
    }

    // get routes
    query = "SELECT COUNT(*) FROM ROUTE";

    try{
      [rows, fields] = await globalConnection.execute(query);
      routes = rows[0]['COUNT(*)'];
    } catch(e){
      console.log('error while getting file count ' + e);
      success = false;
    }

    // get files
    query = "SELECT COUNT(*) FROM POINT";

    try{
      [rows, fields] = await globalConnection.execute(query);
      points = rows[0]['COUNT(*)'];
    } catch(e){
      console.log('error while getting file count ' + e);
      success = false;
    }

    var statusMessage = 'Number of files: ' + files + ', \nNumber of routes: ' + routes + ', \nNumber of points: ' + points;

    console.log( statusMessage );

    await res.send({
      success: success,
      status: statusMessage
    })
  }
});


app.get('/getGPXID', async function(req, res){
  let rows;
  let fields;
  console.log('getting gpxid')
  var gpx_id = 0;

  var fileName = req.query.fileName;

  // get gpx_id
  var findID = "SELECT gpx_id FROM FILE " +
  "WHERE file_name = '" + fileName + "';";

  console.log( findID );

  if( globalConnected ){
    try{
      console.log( findID );
      [rows, fields] = await globalConnection.execute(findID);

      console.log(rows);

      gpx_id = rows[0]['gpx_id'];
    } catch(e){
      console.log('Failed to get gpx_id ' + e );
    } finally {
      res.send({
        gpx_id: gpx_id
      });
    }
  }
});