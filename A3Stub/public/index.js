// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
  // send a request to get the file names
  // then prepopulate the data with each file's info
  jQuery.ajax({
    type: 'get',
    dataType: 'json',
    url: '/retrieveFiles',

    success: function( data ){

      console.log("successfly recieved files from server for startup");

      // for each gpxFile data returned
      for( let gpxFile in data.gpxFilesObject ){
        
        console.log(gpxFile);
        
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
          "<a class='dropdown-item' id='addRouteDropDownItem'>" + gpxFile + "</a>"
        ); 
      }
    }, 

    fail: function(error) {
      console.log("failed");
      console.log(error);
    }
  });

  // event listener gpx view panel to update it whenever new gpx file is selected
  $('#gpxViewPanelSelector').change(function(e){

    e.preventDefault();

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
  });

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
            // make a string out of all the other data
            
            // alert the string
          },

          fail: function( error ){
            console.log('Failed to get otherData for component: ' + name );
            console.log( error );
          }
        });
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
        
          },
    
          fail: function( error ){
            console.log('Failed to get otherData for component: ' + name );
            console.log( error );
          }
        });
      }

     



      // get the data

      // put it in a alert box
    }

  });



  // listener for changing name of a component

    // check if value is track or route

    // check for empty input
    // check for track or route selected

    // make call to right endpoint  



});