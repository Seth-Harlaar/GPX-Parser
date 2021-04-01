// send a request to get the file names
// then prepopulate the data with each file's info
function reloadFiles(){
  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/retrieveFiles',

    success: function( data ){

      console.log("successfly recieved files from server for startup");
      
      $('#filePanelBody').empty();
      $('#gpxViewPanelSelector').empty();
      $('#addRouteSelector').empty();
      
      $('#gpxViewPanelSelector').append(
        '<option selected>Select a File</option>'
      );

      $('#addRouteSelector').append(
        '<option selected>Select a File</option>'
      );

      // for each gpxFile data returned
      for( let gpxFile in data.gpxFilesObject ){
        
        console.log( gpxFile );
        
        // add all the files to the file log panel
        $('#filePanelBody').append(
          "<tr>" + 
            "<th scope='row'><a href='" + gpxFile + "'>" + gpxFile + "</a></th>" + 
            "<td>" + data.gpxFilesObject[gpxFile].version + "</td>" + 
            "<td>" + data.gpxFilesObject[gpxFile].creator + "</td>" +
            "<td>" + data.gpxFilesObject[gpxFile].numWaypoints + "</td>" +
            "<td>" + data.gpxFilesObject[gpxFile].numRoutes + "</td>" +
            "<td>" + data.gpxFilesObject[gpxFile].numTracks + "</td>" + 
          "</tr>"
        );

        // add all the files to the drop down in gpx view Panel
        // and the add route drop down
        $('#gpxViewPanelSelector').append(
          "<option value='" + gpxFile + "'>" + gpxFile + "</option>"
        );

        $('#addRouteSelector').append(
          "<option value='" + gpxFile + "'>" + gpxFile + "</option>"
        ); 
      }
    }, 

    fail: function(error) {
      console.log("failed");
      console.log(error);
    }
  });

}


function reloadViewPanel(){
  // get the file selected from the dropdown menu
  var dropDownName = $('#gpxViewPanelSelector').val();

  // send a request to get the JSON object representing all the components
  console.log('Making request for compenent data for: ' + dropDownName );

  // reset the other data button and change name form
  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/getRoutes',
    data: {
      name: dropDownName
    },

    success: function( data ){
      console.log( data );
      // for each route found add the components
      var i = 1;
        
      $('#gpxViewBody').empty();


      for( let route in data.gpxRoutesObject ){
        console.log( route );
        $('#gpxViewBody').append(
          "<tr id='true' value='" + data.gpxRoutesObject[route].name + "'>" +
            "<th scope='row'>" + 'Route ' + i + "</th>" +
            "<td>" + data.gpxRoutesObject[route].name + "</td>" +
            "<td>" + data.gpxRoutesObject[route].numPoints + "</td>" +
            "<td>" + data.gpxRoutesObject[route].len + "</td>" +
            "<td>" + data.gpxRoutesObject[route].loop + "</td>" +
          "</tr>"
        );
        i++;
      }
    },

    fail: function( error ){
      console.log('Failed to get routes for file:' + dropDownName );
      console.log( error );
    }
  })

  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/getTracks',
    data: {
      name: dropDownName
    },

    success: function( data ){
      console.log( data );
      // for each route found add the components
      var i = 1;

      for( let track in data.gpxTracksObject ){
        console.log( track );
        $('#gpxViewBody').append(
          "<tr id='false' value='" + data.gpxTracksObject[track].name + "'>" +
            "<th scope='row'>" + 'Track ' + i + "</th>" +
            "<td>" + data.gpxTracksObject[track].name + "</td>" +
            "<td>" + data.gpxTracksObject[track].numPoints + "</td>" +
            "<td>" + data.gpxTracksObject[track].len + "</td>" +
            "<td>" + data.gpxTracksObject[track].loop + "</td>" +
          "</tr>"
        );
        i++;
      }
    },

    fail: function( error ){
      console.log('Failed to get tracks for file:' + dropDownName );
      console.log( error );
    }
  })
}



// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
  reloadFiles();

  // event listener gpx view panel to update it whenever new gpx file is selected
  $('#gpxViewPanelSelector').change(function(e){

    e.preventDefault();

    reloadViewPanel();
  });




  // upload file form
  $('#uploadForm').submit(function(e){

    if( $('#fileInput')[0].files.length === 0 ){
      e.preventDefault();
      alert('No input files');
      console.log('no input files');
    }

    // check if the name has a gpx extension
    // how to get file extension from here: https://stackoverflow.com/questions/190852/how-can-i-get-file-extensions-with-javascript/1203361#1203361
    var fileName = $('#fileInput')[0].files[0].name;
    var fileExtension = fileName.substring(fileName.lastIndexOf('.') + 1, fileName.length) || fileName;

    if( fileExtension != 'gpx' ){
      alert('Not a GPX file. ');
      e.preventDefault();
    }
    console.log( 'file extension: ' + fileExtension );
  });




  // add listeners for routes/tracks in the gpx view panel to rename them
  $(document).on('click', '#gpxViewBody tr', function(){
    
    var route = ( $(this).attr('id') == 'true');
    var name = $(this).attr('value');

    if( route ) {
      // rename function for routes
      $('#renameSpot').html(
        '<div value="route">' + name + '</div>'
      )
      // set text and value
      $('#otherDataButton').val('true');
      $('#otherDataButton').text(name);

    } else {
      // rename function for tracks
      $('#renameSpot').html(
        '<div value="track">' + name + '</div>'
      )
    
      // set text and value
      $('#otherDataButton').val('false');
      $('#otherDataButton').text(name);

    }
  });






  // listener for viewing other data of a component
  $('#otherDataButton').click(function(e){
    // get text
    var name = $('#otherDataButton').text();

    var fileName = $('#gpxViewPanelSelector').val();

    console.log(fileName);

    var route = ( $('#otherDataButton').attr('value') == 'true');

    if( route ){
      console.log( 'getting other data for route named: ' + name );
    } else {
      console.log( 'getting other data for track named: ' + name );
    }

    // check that value is not __none
    if( $('#otherDataButton').attr('value') == '__none'){
      alert('Please select a route from above first.');
    } else {
      // make request to endpoint

      // if route
      if( route ){
        // if track
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/getOtherData',
          data: {
            name: name,
            fileName: fileName,
            route: true

          },

          success: function( data ){
            var alertString = 'Other data: \n';
            // make a string out of all the other data
            if( data.success == true ){

              for( let stuff in data.content ){
                alertString = alertString + "\n" + data.content[stuff].name + ": " + data.content[stuff].value;
              }
              alert( alertString );

            } else {
              alert('There was an error retrieving the other data');
            }
          },

          fail: function( error ){
            console.log('Failed to get otherData for component: ' + name );
            console.log( error );
          }
        });

      // if track
      } else {
        // if track
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/getOtherData',
          data: {
            name: name,
            fileName: fileName,
            route: false
          },
    
          success: function( data ){
            var alertString = 'Other data: \n';
            // make a string out of all the other data
            if( data.success == true ){

              for( let stuff in data.content ){
                alertString = alertString + "\n" + data.content[stuff].name + ": " + data.content[stuff].value;
              }
              alert( alertString );

            } else {
              alert('There was an error retrieving the other data');
            }
          },
    
          fail: function( error ){
            console.log('Failed to get otherData for component: ' + name );
            console.log( error );
          }
        });
      }
    }
  });




  // listener for making new gpx file

  $('#createNewDocForm').submit(function(e){
    e.preventDefault();

    // get the file name, creator, and version
    var fileName = $('#newDocName').val();
    var creator = $('#newDocCreator').val();
    // check that it is a double
    var version = $('#newDocVersion').val();

    // check first that file's extension is gpx
    // solution to get extension found here: https://stackoverflow.com/questions/190852/how-can-i-get-file-extensions-with-javascript/1203361#1203361
    var fileExtension = fileName.substring(fileName.lastIndexOf('.') + 1, fileName.length) || fileName;

    if( fileExtension != 'gpx'){
      alert('New File name must end in ".gpx"');
    
    } else if( isNaN(version) ){
      alert('Version input must be a float');

    } else {
      $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/newDoc',
        data: {
          fileName: fileName,
          creator: creator,
          version: version
        }, 
        

        success: function( data ){
          reloadFiles();
          alert('sucecss');
        }, 
  
        fail: function( error ){
          console.log( error );
        }
      })
    }
  });



  // update the information in the new route form
  $('#addRouteSelector').change(function(e){

    var fileName = $('#addRouteSelector').val();

    $('#addRouteDocLabel').html(
      fileName
    );
    $('#addRouteDocLabel').value(fileName);
  });




  // update the information in the new route form -> add waypoints 
  $('#addWptsForm').submit(function(e){
    e.preventDefault();

    $('#wptsFormSpot').empty();

    var numWpts = parseInt( $('#numberOfWpts').val() );
    var i;

    if( numWpts < 0 || isNaN(numWpts) ){
      alert('Please provide an integer greater than or equal to 0' );
    } else {
      console.log('adding ' + numWpts + ' waypoints' );
      // set the value
      $('#wptsFormSpot').append(
        '<div id="wptsCountHolder" value="'+ numWpts +'"></div>'
      );
      // fill in with the form spots
      for( i = 0; i < numWpts; i++ ){
        $('#wptsFormSpot').append(
          '<h5 class="mt-5"> Waypoint' + i + ':</h5>' +
          '<div id="newWpt">' +
            '<div class="row mt-3">' +
              '<div class="col">' +
                '<input type="text" class="form-control" name="lat" placeholder="Latitude">' +
              '</div>' +
              '<div class="col">' +
                '<input type="text" class="form-control" name="lon" placeholder="Longitude">' +
              '</div>' +
            '</div>' +
          '</div>'
        );
      }
    }
  });



  $('#newRouteForm').submit(function(e){
    e.preventDefault();

    var submit = true;

    var i = 0;

    // get the new route name and the file to add it to
    var fileName = $('#addRouteSelector').val();

    var newRouteName = $('#newRouteName').val();
    // get all the info out of the waypoint form
    
    var wptData = {};
    var wptLat = [];
    var wptLon = [];

    // for every div with id tag newWpt, get the info out and add it to wptData
    var newRouteData = $('#newRouteForm').serializeArray();
    var num;

    newRouteData.forEach( item =>{
      // handle new route name input
      if( item.name == 'newRouteName' ){
        wptData['newRouteName'] = item.value;
      } else {

        // handle lat input
        if( item.name == 'lat' ){
          num = parseFloat( item.value );

          if( isNaN(num) || num > 90.0 || num < -90.0 ){
            submit = false;
            alert('Input latitude must be a float between (and including) 90.0 and -90.0');
          }

          wptLat[i] = item.value;

        // handle lon input
        } else if( item.name == 'lon' ){
          num = parseFloat( item.value );

          if( isNaN(num) || num > 180.0 || num < -180.0 ){
            submit = false;
            alert('Input longitude must be a float between (and including) 180.0 and -180.0');
          }

          wptLon[i] = item.value;
          i++;
        }
      }
    });

    wptData['wpts'] = {};

    for( i = 0; i < wptLat.length; i++ ){

      wptData.wpts['wpt' + i] = {
        lat: parseFloat(wptLat[i]),
        lon: parseFloat(wptLon[i])
      }
    }

    console.log( wptData );

    if( !newRouteName ){
      alert( 'Please provide a name for the new route. ');
    
    } else if( submit ) {
      $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/newRoute',
        data: {
          wptData: wptData,
          fileName: fileName
        }, 

        success: function(data) {
          if( data.success == 'true' ){
            reloadViewPanel();
            reloadFiles();
            console.log( 'successfully added new route to file');
          } else {
            alert('failed to add new route to file');
          }
        }
      });
    }
  });



  // listener for changing name of a component
  $('#changeNameForm').submit(function(e){
    e.preventDefault();

    // check if value is track or route
    var route = ( $('#otherDataButton').attr('value') == 'true');

    var newName = $('#newElementName').val();
    var oldName = $('#otherDataButton').text();

    var fileName = $('#gpxViewPanelSelector').val();

    // check for empty input
    if( !newName ){
      alert('Please input a new name'); 
    
    } else if( $('#otherDataButton').attr('value') == '__none' ){
      alert('Please select a component first');
    
    } else {
      // check for track or route selected
      if( route ){
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/renameRoute',
          data :{
            oldName: oldName,
            newName: newName,
            fileName: fileName
          }, 

          success: function( data ){
            if( data.success == 'true' ){
              reloadViewPanel();
              console.log( 'successfully changed name of route ' );
            } else {
              alert('Failed to change name of route');
            }
          }
        })

      } else {
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/renameTrack',
          data :{
            oldName: oldName,
            newName: newName,
            fileName: fileName
          }, 

          success: function( data ){
            if( data.success == 'true' ){
              reloadViewPanel();
              console.log( 'successfully changed name of track ' );
            } else {
              alert('Failed to change name of track');
            }
          }
        })
      }
    }
  });


  $('#pathBetweenForm').submit(function(e){
    e.preventDefault();

    var submit = true;

    // need lat long and tolerance
    var lat1 = parseFloat( $('#inputLat1').val() );
    var lon1 = parseFloat( $('#inputLon1').val() );
    var lat2 = parseFloat( $('#inputLat2').val() );
    var lon2 = parseFloat( $('#inputLon2').val() );
    var tol = parseFloat( $('#inputTol').val() );

    // check input
    if( lat1 > 90.0 || lat1 < -90.0 || lat2 > 90.0 || lat2 < -90.0 ){
      alert('Input latitude must be a float between (and including) 90.0 and -90.0.');
      submit = false;
    }

    if( lon1 > 180.0 || lon1 < -180.0 || lon2 > 180.0 || lon2 < -180.0 ){
      alert('Input Longitude must be a float between (and including) 180.0 and -180.0');
      submit = false;
    }

    if( isNaN(lat1) || isNaN(lon1) || isNaN(tol) || isNaN(lat2) || isNaN(lon2) ){
      alert('Inputs must be all be numbers');
      submit = false;
    }

    if( submit ){
      console.log('Making reqest to find routes/tracks between points ');

      // get all the file names
      $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/retrieveFiles',

        success: function( data ){
          // make a request for each
          for( let gpxFile in data.gpxFilesObject ){

            // send the data
            $.ajax({
              type: 'get',
                dataType: 'json',
                url: '/pathsBetweenRoutes',
                data :{
                  fileName: gpxFile,
                  lat1: lat1,
                  lat2: lat2,
                  lon1: lon1,
                  lon2: lon2,
                  tol: tol
                }, 
      
                success: function( data ){
                  if( data.success == true){
                    console.log('successfully retrieved all routes/tracks between point');
                    console.log( data );
                  } else {
                    alert('failed to find routes/tracks between point');
                  }
                } 
            });
          }
        }
      })



    }
  });
});


