// send a request to get the file names
// then prepopulate the data with each file's info
function reloadFiles(){
  var valid; 

  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/retrieveFiles',

    success: function( data ){

      console.log("successfly recieved files from server for startup");
      
      $('#filePanelBody').empty();
      $('#gpxViewPanelSelector').empty();
      $('#addRouteSelector').empty();
      $('.fileSelector').empty();
      
      $('#gpxViewPanelSelector').append(
        '<option selected>Select a File</option>'
      );

      $('#addRouteSelector').append(
        '<option selected>Select a File</option>'
      );

      $('.fileSelector').append(
        '<option selected>Select a File</option>'
      );

      console.log(data)

      if( data.count != 0 ){
        console.log('length');
        $('#saveFilesButtonSpot').append(
          '<button type="submit" id="saveFilesButton" class="btn btn-info">Save Files</button>'
          );
        } else {
        console.log('no length');
      }

      // for each gpxFile data returned
      for( let gpxFile in data.gpxFilesObject ){

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

        $('.fileSelector').append(
          "<option value='" + gpxFile + "'>" + gpxFile + "</option>"
        ); 
        
        console.log( gpxFile );
      }
    }, 

    fail: function(error) {
      console.log("failed");
      console.log(error);
    }
  });
}

function reloadRoutes(fileName){
  // get each routes info
  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/getRoutes',
    data: {
      name: fileName
    },

    success: function(data){
      $('#allPointsFromRoute').empty();

      if(data.gpxRoutesObject.length != 0){
        $('#allPointsFromRoute').append(
          "<option>Select a Route</option>"
        );
      } else {
        $('#allPointsFromRoute').append(
          "<option>No Routes for File</option>"
        );
      }
      

      for(let route in data.gpxRoutesObject){
        $('#allPointsFromRoute').append(
          "<option value='" + data.gpxRoutesObject[route].name + "'>" + data.gpxRoutesObject[route].name + "</option>"
        );
      }
    }
  });
}


function reloadViewPanel(){
  // get the file selected from the dropdown menu
  var dropDownName = $('#gpxViewPanelSelector').val();

  if( dropDownName != 'Select a File'){
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
  
        if( data.gpxRoutesObject.length != 0 ){
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
  
        if( data.gpxTracksObject.length != 0 ){
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
        }
      },
  
      fail: function( error ){
        console.log('Failed to get tracks for file:' + dropDownName );
        console.log( error );
      }
    })
  }

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
    } else {
      // check if the name has a gpx extension
      // how to get file extension from here: https://stackoverflow.com/questions/190852/how-can-i-get-file-extensions-with-javascript/1203361#1203361
      var fileName = $('#fileInput')[0].files[0].name;
      var fileExtension = fileName.substring(fileName.lastIndexOf('.') + 1, fileName.length) || fileName;
  
      if( fileExtension != 'gpx' ){
        alert('Not a GPX file. ');
        e.preventDefault();
      }
      console.log( 'file extension: ' + fileExtension );
    }
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


    if( fileName == 'Select a File' ){
      alert('Please select a file from above');
    } else {
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
    }
  });


  // listener for name on file input -- taken from w3schools here:
  // https://www.w3schools.com/bootstrap4/bootstrap_forms_custom.asp
  $(".custom-file-input").on("change", function() {
    var fileName = $(this).val().split("\\").pop();
    $(this).siblings(".custom-file-label").addClass("selected").html(fileName);
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
          console.log('Successfully added new file to /uploads/');
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

            // at the end save a new route to db
            saveNewRoute( fileName, newRouteName );
          } else {
            alert('please select a file to add route to ');
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



// get routes between two points
  $('#pathBetweenForm').submit(function(e){
    e.preventDefault();

    var submit = true;
    var i = 1;
    var j = 1;

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

            // send the data to get routes
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
                  // if it works
                  if( data.success == true){
                    // if there is data
                    if( data.empty != true ){
                      // empty the table
                      
                      // for each route found in the data
                      for( let route in data.routeData ){

                        console.log( data.routeData[route] );
                        // add it to the table as a row
                        $('#routesBetweenViewBody').append(
                          "<tr id='true' value='" + data.routeData[route].name + "'>" +
                            "<th scope='row'>" + 'Route ' + i + "</th>" +
                            "<td>" + data.routeData[route].name + "</td>" +
                            "<td>" + data.routeData[route].numPoints + "</td>" +
                            "<td>" + data.routeData[route].len + "</td>" +
                            "<td>" + data.routeData[route].loop + "</td>" +
                          "</tr>"
                        );
                        i++;
                      }
                    }
                    console.log( data );
                  } else {
                    alert('failed to find routes between points');
                  }
                } 
            });

            // get the data for all the tracks
            $.ajax({
              type: 'get',
              dataType: 'json',
              url: '/pathsBetweenTracks',
              data: {
                fileName: gpxFile,
                  lat1: lat1,
                  lat2: lat2,
                  lon1: lon1,
                  lon2: lon2,
                  tol: tol
              },

              // 
              success: function( data ){
                // if it worked
                if( data.success == true ){
                  // if there is data
                  if( data.empty != true ){

                    // for each track found in the data
                    for( let track in data.trackData ){

                      console.log( data.trackData[track] );
                      // add it to the table as a row
                      $('#routesBetweenViewBody').append(
                        "<tr id='true' value='" + data.trackData[track].name + "'>" +
                          "<th scope='row'>" + 'Track ' + j + "</th>" +
                          "<td>" + data.trackData[track].name + "</td>" +
                          "<td>" + data.trackData[track].numPoints + "</td>" +
                          "<td>" + data.trackData[track].len + "</td>" +
                          "<td>" + data.trackData[track].loop + "</td>" +
                        "</tr>"
                      );
                      i++;
                    }

                  }
                } else {
                  alert('failed to find tracks between points');
                }
              }
            })
          }
        }
      })
    }
  });
});



// **************** A4 funcitonality **********************
// functions

// display # of rows in each table
async function status(){
  await $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/getStatus',

    success: async function(data){
      if( data.success == true ){
        alert( data.status );
      }
    }
  });
}


async function saveFiles(){
  var creator;
  var version;

  console.log('saving info for files');
  // retrieve the valid files
  await $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/retrieveFiles',

    success: async function( data ){
      
      // for each file, get its data n stuff
      for( let gpxFile in data.gpxFilesObject ){
        version = data.gpxFilesObject[gpxFile].version;        
        creator = data.gpxFilesObject[gpxFile].creator;
        fileName = gpxFile;

        // send the info to save the file
        await $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/saveFile',
          data: {
            version: version,
            creator: creator,
            fileName: fileName
          },

          // if the file was successfully saved
          success: async function( data ){
            if( data.success == true ){
              console.log('saved file');

              var gpx_id = data.gpx_id;
              
              // save the routes of the file
              await saveRoutes( fileName, gpx_id );
            }
          }
        });
      }
    }
  })
}


// save all the routes of a file
async function saveRoutes( fileName, gpx_id ){
  // get each route for the file
  console.log('saving routes for '+ fileName + ' with gpx_id: '+ gpx_id );

  await $.ajax({
    type: 'get', 
    dataType: 'json',
    url: '/getRoutes',
    data: {
      name: fileName
    },

    success: async function( data ){

      // check if there are any routes
      if( data.gpxRoutesObject.length != 0 ){
        // for each route in the obj
        for( let route in data.gpxRoutesObject ){
          // get the info
          var routeLength = data.gpxRoutesObject[route].len;
          var routeName = data.gpxRoutesObject[route].name;
          
          // make a call to server to save the route
          await $.ajax({
            type: 'get', 
            dataType: 'json',
            url: '/saveRoute',
            data: {
              name: routeName,
              routeLength: routeLength,
              gpx_id: gpx_id
            },

            success: async function(data){
              if( data.success == true ){
                console.log('Successfully saved route named: ' + routeName );
              }

              // get the route id
              var route_id = data.route_id;

              // save the routes point info
              await savePoints( routeName, fileName, route_id );
            }
          })
        }
      }
    }
  });
}

// save only one route, the new one
async function saveNewRoute( fileName, newRouteName ){
  // get the gpx_id, then do same as other saveRoutes
  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/getGPXID',
    data: {
      fileName: fileName
    },
    
    success: async function(data){
      // if a gpx_id was found
      if( data.gpx_id != 0 ){
        // make a call to get all the route info for the file
        var gpx_id = data.gpx_id;
        await $.ajax({
          type: 'get', 
          dataType: 'json',
          url: '/getRoutes',
          data: {
            name: fileName
          },
          
          success: async function( data ){
      
            // check if there are any routes
            if( data.gpxRoutesObject.length != 0 ){
              // for each route in the file
              for( let route in data.gpxRoutesObject ){
                // only run if the the name is the same as the new route we are looking for
                if( data.gpxRoutesObject[route].name == newRouteName ){
      
                  // get the info
                  var routeLength = data.gpxRoutesObject[route].len;
                  var routeName = data.gpxRoutesObject[route].name;
                  
                  // make a call to server to save the route
                  await $.ajax({
                    type: 'get', 
                    dataType: 'json',
                    url: '/saveRoute',
                    data: {
                      name: routeName,
                      routeLength: routeLength,
                      gpx_id: gpx_id
                    },
      
                    success: async function(data){
                      if( data.success == true ){
                        console.log('Successfully saved route named: ' + routeName );
                      }
      
                      // get the route id
                      var route_id = data.route_id;
      
                      // save the routes point info
                      await savePoints( routeName, fileName, route_id );
                    }
                  })
                }
              }
            }
          }
        });
      }
    }
  });
}

// save all the point data for each route
async function savePoints( routeName, fileName, route_id ){
  // for each route found in the file
  // send a request to get the wpt data
  console.log('saving points for route: ' + routeName + ' from file: ' + fileName );
  await $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/getPoints',
    data: {
      routeName: routeName,
      fileName: fileName
    },

    success: async function(data){
      if( data.success == true ){
        console.log( 'successfully retrieved point data for: ' + routeName );
        console.log( data );

        // for each point in the route data
        for( let point in data.gpxPointData ){

          await $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/savePoint',
            data: {
              index: point,
              lat: data.gpxPointData[point].lat,
              long: data.gpxPointData[point].lon,
              pointName: data.gpxPointData[point].name,
              route_id: route_id
            }, 

            success: function(data){
              console.log('successfully saved point to db');
            }
          })
        }
      }
    }
  })
}


// listeners

// listener for saving all the files
$(document).on('click', '#saveFilesButton', async function(e){
  e.preventDefault();

  await saveFiles();

  await status();
});



// listener for logging the user in
$('#dbLoginForm').submit(function(e){

  e.preventDefault();

  // get user info
  var userName  = $('#dbUserName').val();
  var password  = $('#dbPassword').val();
  var dbName    = $('#dbName').val();


  console.log( userName + password + dbName );

  // send it to the server
  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/login',
    data: {
      userName: userName,
      password: password,
      dbName: dbName
    },

    success: function( data ){

      if( data.success == true ){
        alert('successfully connected');

        $('#logStatus').html('Currently: Logged In');

        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/createTables',
          
          success: function(data){
            if( data.success ){
              console.log('successfully created all tables');
            }
          }
        })

      } else {
        alert('Could not connect to database, please re-enter information and try again.');
      }
    }
  });
});

// listener for logging the user out
$(document).on('click', '#logoutButton', function(){

  $.ajax({
    type: 'get',
    dataType: 'json',
    url: '/logout',
    
    success: function(data){
      alert('disconnected from server');
      $('#logStatus').html('Currently: Logged Out');
    }
  });

});

// listener for clearing all data
$(document).on('click', '#clearDataButton', async function(){
  await $.ajax({
    type: 'get', 
    dataType: 'json',
    url: '/clearData',
    
    success: function(data){
      if( data.success){
        console.log('Successfully cleared all data from db ');
      }
    }
  });
  await status();
});

$(document).on('click', '#statusButton', async function(){
  console.log('displaying status');
  await status();  
});



$('#allPointsFromRouteFileSelector').change(function(e){
  var fileName = $('#allPointsFromRouteFileSelector').val();

  reloadRoutes(fileName);
});



// query stuff

// function to run the query
async function executeQuery( queryString ){
  var rowData;

  // if it not empty
  if( queryString ){
    await $.ajax({
      type: 'get', 
      dataType: 'json',
      url: '/execute',
      data: {
        queryString: queryString
      }, 
      
      success: async function(data){
        if(data.success == true){
          rowData = data.rowData;
        }
      }
    })
  } 
  return rowData;
}

// function to display the result in table
function displayResults(data){
  console.log(data);

  // empty the table
  $('#resultTableColHeaders').empty();
  $('#resultTableBody').empty();


  

  if(data.length != 0){

    // fill in the table headers
    for( let header in data[0] ){
      $('#resultTableColHeaders').append(
        "<th scope='col'>" + header + "</th>"
      );
    }

    // for each element in the array
    for( let item in data ){
      
      // add the openning table row tag
      $('#resultTableBody').append(
        "<tr>" 
      );
        
      // add all the data one by one
      for( let info in data[item] ){
        $('#resultTableBody').append(
          "<td>" + data[item][info] + "</td>"
        );
      }

      // add the closing table row tag
      $('#resultTableBody').append(
        "</tr>"
      );
    }     
  } else {
    // display no data on the table
    $('#resultTableColHeaders').append(
      "<th scope='col'> No Results </th>"  
    );
  }
}




// listeners for the queries

// query 1
$('#displayAllRoutes').submit( async function(e){
  e.preventDefault();

  var sortKeyWord;

  // get the form info
  var sort = $('input[name=sort]:checked', '#displayAllRoutes').val();

  // make query
  if( sort == 'name' ){
    sortKeyWord = 'route_name'
  } else {
    sortKeyWord = 'route_len'
  }

  var allRoutes = "SELECT * FROM ROUTE " +
  "ORDER BY " + sortKeyWord;

  allRouteData = await executeQuery(allRoutes);

  displayResults(allRouteData);
});


// query 2
$('#displayAllRoutesFromFile').submit(async function(e){
  e.preventDefault();

  var sortKeyWord;

  var sort = $('input[name=sort]:checked', '#displayAllRoutesFromFile').val();

  var dropDownName = $('#allRoutesFromFileSelector').val();

  if( sort == 'name' ){
    sortKeyWord = 'route_name'
  } else {
    sortKeyWord = 'route_len'
  }

  if( dropDownName == 'Select a File'){
    alert('Please select a file first');
  } else {
    // get the gpx_id first
    $.ajax({
      type: 'get',
      dataType: 'json',
      url: '/getGPXID',
      data: {
        fileName: dropDownName
      }, 
  
      success: async function(data){
        var gpx_id = data.gpx_id;
  
        if( gpx_id != 0 ){
          // make the query
          var allRoutesFromFile = "SELECT * FROM ROUTE "+
          " WHERE gpx_id = " + gpx_id + 
          " order by " + sortKeyWord;
  
          console.log(allRoutesFromFile);

          // execute and display results
          routeData = await executeQuery(allRoutesFromFile);
  
          displayResults(routeData);
        }
  
      }
    })
  }
});


// query 3
$('#displayAllPointsFromRoute').submit( async function(e){
  e.preventDefault();

  var gpx_id;
  var route_id;

  // get the route name
  var routeName = $('#allPointsFromRoute').val();
  
  // get the file name
  var fileName = $('#allPointsFromRouteFileSelector').val();

  // find gpx_id and route_id
  if(routeName != 'Select a Route' && routeName != 'No Routes for File'){
    await $.ajax({
      type: 'get',
      dataType: 'json',
      url: '/getGPXID',
      data: {
        fileName: fileName
      }, 
  
      success: async function(data){
        if( data.gpx_id != 0 ){
          gpx_id = data.gpx_id;
  
          await $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getRouteID',
            data: {
              gpx_id: gpx_id,
              routeName: routeName
            }, 
            
            success: async function(data){
              if( data.route_id != 0 ){
                var route_id = data.route_id;
  
                // make query out of the info
                var getPoints = "SELECT * FROM POINT " +
                " WHERE route_id = " + route_id;
  
                var pointData = await executeQuery(getPoints);
  
                displayResults(pointData);
              }
            }
          })
        }
      }
    })
  } else {
    alert('Please select a route first');
  }
});


// query 4
$('#displayAllPointsFromFile').submit(async function(e){
  e.preventDefault();
  
  var gpx_id;

  // get radio value
  var sort = $('input[name=sort]:checked', '#displayAllPointsFromFile').val();

  if(sort == 'name'){
    var sortKeyWord = 'route_name'
  } else {
    var sortKeyWord = 'route_len'
  }

  // get selected file
  var fileName = $('#allPointsFromFileSelector').val();

  if( fileName != 'Select a File' ){

    // get the gpx_id
    await $.ajax({
      type: 'get',
      dataType: 'json',
      url: '/getGPXID',
      data: {
        fileName: fileName
      }, 

      success: async function(data){
        if(data.gpx_id != 0){
          gpx_id = data.gpx_id;

          // make query
          var getPoints = "SELECT DISTINCT point_id, latitude, longitude, point_index, route_name, route_len " +
          " FROM FILE, ROUTE, POINT " +
          " WHERE (FILE.file_name = '" + fileName + "' and FILE.gpx_id=ROUTE.gpx_id and ROUTE.route_id=POINT.route_id) " +
          " order by " + sortKeyWord + ", POINT.route_id, point_index ";

          let pointData = await executeQuery(getPoints);
        
          displayResults(pointData);
        }
      }
    })

  } else {
    alert('Please select a file first')
  }
});


// query 5
$('#displayNRoutesFromFile').submit( async function(e){
  e.preventDefault();

  var all = false;
  var ex = true;

  // get the sort
  var sort = $('input[name=sort]:checked', '#displayNRoutesFromFile').val();
  var order = $('input[name=order]:checked', '#displayNRoutesFromFile').val();

  if(order == 'short'){
    var sortKeyWord = 'ASC'
  } else {
    var sortKeyWord = 'DESC'
  }
  
  if(sort == 'name'){
    var sortKeyWord2 = 'route_name'
  } else {
    var sortKeyWord2 = 'route_len'
  }

  // get the number of routes
  var numberOfRoutes = $('#numberOfRoutes').val();


  // get the filename
  var fileName = $('#NRoutesFileSelector').val();

  // check if n is 'all' or a number
  if(numberOfRoutes != 'all'){
    // get the int value if its not all
    var n = parseInt(numberOfRoutes);
    // check for positive integer
    if( n < 1 || isNaN(numberOfRoutes)){
      alert('Please Enter an integer greator than 0 or "all" to display all the routes');
      ex = false;
    }
  } else {
    all = true;
  }

  // check if should execute or not based on the input
  if(ex){
    if(fileName != 'Select a File'){

      // select all the records
      if(all){
        // make the query
        var allRoutesFromFile = "SELECT route_id, route_name, route_len, file_name FROM ROUTE, FILE "+
        " WHERE FILE.gpx_id=ROUTE.gpx_id AND FILE.file_name = '" + fileName + "' " +
        " ORDER BY route_len, " + sortKeyWord2; 

        // execute and display results
        routeData = await executeQuery(allRoutesFromFile);

        displayResults(routeData);

      // select a certian number of them
      } else {

        var someRoutesFromFile = "SELECT route_id, route_name, route_len, file_name FROM ROUTE, FILE "+
        " WHERE FILE.gpx_id=ROUTE.gpx_id AND FILE.file_name = '" + fileName + "' " +
        " ORDER BY route_len " + sortKeyWord  + ", " + sortKeyWord2 +
        " LIMIT " + n;

        routeData = await executeQuery(someRoutesFromFile);

        displayResults(routeData);
      }

    } else {
      alert('Please select a file first');
    }
  }
});