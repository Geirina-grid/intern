import { Injectable } from '@angular/core';
import { WholeGraphService } from '../../services/whole-graph.service';
import { notEqual } from 'assert';
import { environment } from 'environments/environment';
import { Headers, Http } from '@angular/http';
@Injectable()
export class DemodataService {
  private gsqlPre = environment.gsqlPre;
  mapbox_token = 'pk.eyJ1IjoiamFja21hbzE5OTAiLCJhIjoiY2ppdzd6M2MzMXVwYjNxa2JkbDB2MHd6OSJ9.3oryG7TA-tqF2qTgReMwwg';
  constructor(private http: Http) { }
  LINE_COORD: any;


  // linkgenerator(nodedata) {
  //   console.log(nodedata);

  // }

  // LINE_COORD = this.linkgenerator(nodeinfo);  // linkgenerator(nodedata) {
  //   console.log(nodedata);

  // }

  // LINE_COORD = this.linkgenerator(nodeinfo);

  
  NodeGenerator(coord) {
    // console.log(coord);
    var res = [];
    for (var i = 0; i < coord[1].T0.length; i++) {
      res.push({        
        name: coord[1].T0[i].v_id,
        value: [+coord[1].T0[i].attributes.pos_y, +coord[1].T0[i].attributes.pos_x]
      })
    }
    return res
  }
  
  LinkGenerator(result) {
    var linklist = [];
    var nodelist = this.NodeGenerator(result);
    var singlelink = [];
    var rawlink =  result[0]['@@edgeList'];
    for (var i = 0; i < rawlink.length; i++) {
      if (rawlink[i].attributes['Int_1_x'] == 0) {
        singlelink = [nodelist.find(x => x.name == rawlink[i]['from_id']).value, nodelist.find(x => x.name == rawlink[i]['to_id']).value];
        linklist.push(singlelink);
      } else {
        var internode1 = [rawlink[i].attributes['Int_1_y'], rawlink[i].attributes['Int_1_x']];
        var internode2 = [rawlink[i].attributes['Int_2_y'], rawlink[i].attributes['Int_2_x']];
        var internode3 = [rawlink[i].attributes['Int_3_y'], rawlink[i].attributes['Int_3_x']];
        var interquestion = [internode1, internode2, internode3];
        for (var _ in interquestion) {
          if (true) {
            interquestion.pop();
          }
        }
        interquestion.unshift(nodelist.find(x => x.name == rawlink[i]['from_id']).value);
        interquestion.push(nodelist.find(x => x.name == rawlink[i]['to_id']).value);
        linklist.push(interquestion);
      }
    }
    return linklist
  }





  getLineGeoJson(result, pointdata) {
    // this.LINE_COORD = result;
  	// var coordinates = this.LINE_COORD;
  	// var type = "MultiLineString";
    // var properties = {};
    
  	// var geoJson = new GeoJson(coordinates, properties, type);
    // return geoJson;
    
    const type = "FeatureCollection";
    var features = [];
    // console.log(result[0]['@@edgeList']);
    // console.log(pointdata);
    
    for (var i = 0; i<result[0]['@@edgeList'].length; i++) {

        var linesegment = [];
        var interpoint_x_list = [result[0]['@@edgeList'][i]['attributes']['Int_1_x'], result[0]['@@edgeList'][i]['attributes']['Int_2_x'], result[0]['@@edgeList'][i]['attributes']['Int_3_x']];
        var interpoint_y_list = [result[0]['@@edgeList'][i]['attributes']['Int_1_y'], result[0]['@@edgeList'][i]['attributes']['Int_2_y'], result[0]['@@edgeList'][i]['attributes']['Int_3_y']];
        linesegment.push(pointdata.features.find(x => x.properties.ID == result[0]['@@edgeList'][i]['from_id']).geometry.coordinates);
        for (var j in interpoint_x_list) {
          if (interpoint_x_list[j] != 0) {
            // console.log('oo');
            linesegment.push([interpoint_y_list[j], interpoint_x_list[j]]);
          }
        }
        linesegment.push(pointdata.features.find(x => x.properties.ID == result[0]['@@edgeList'][i]['to_id']).geometry.coordinates);

        for (var k=0; k<(linesegment.length-1); k++) {
          var feature = {};
          feature['type'] = 'Feature';
          feature['geometry'] = {
          'type': 'LineString',
          'coordinates': [linesegment[k], linesegment[k+1]]
          };
          if (+result[0]['@@edgeList'][i].attributes.Config) {
            if (+result[0]['@@edgeList'][i].attributes.Config!=12) {
              var color = '#008C00';
            } else {
              var color = '#356AA0';
            }
          } else {
            var color = '#356AA0';
          }
          feature['properties'] = {
          'FROM_ID': result[0]['@@edgeList'][i].from_id,
          'TO_ID': result[0]['@@edgeList'][i].to_id,
          'CONFIG': +result[0]['@@edgeList'][i].attributes.Config,
          'LENGTH': result[0]['@@edgeList'][i].attributes.linelength,
          'SEGMENT': k,
          'COLOR': color,
          'MIDDLE_POINT': [(linesegment[k][0]+linesegment[k+1][0])/2, (linesegment[k][1]+linesegment[k+1][1])/2]
          }

          features.push(feature);
          // console.log(result[0]['@@edgeList'][i].attributes.Config);
        }


        
        // var feature = {};
        // feature['type'] = 'Feature';
        // feature['geometry'] = {
        //   'type': 'LineString',
        //   'coordinates': linesegment
        // };
        // feature['properties'] = {
        //   'FROM_ID': result[0]['@@edgeList'][i].from_id,
        //   'TO_ID': result[0]['@@edgeList'][i].to_id,
        //   'CONFIG': result[0]['@@edgeList'][i].attributes.Config,
        //   'LENGTH': result[0]['@@edgeList'][i].attributes.linelength 
        // }

        // features.push(feature);

        // console.log(result[0]['@@edgeList'][i]);
    }

    // var geoJson = new GeoJson();
    // console.log(features);
  	return {'type': type, 'features': features};
  }

  getPointsGeoJson(result) {
    // var type = "MultiPoint";
    // var properties = {};
    // console.log(result);
    const type = "FeatureCollection";
    var features = [];
    for (var i = 0; i<result[1].T0.length; i++) {
        var feature = {};
        feature['type'] = 'Feature';
        feature['geometry'] = {
          'type': 'Point',
          'coordinates': [result[1].T0[i].attributes.pos_y, result[1].T0[i].attributes.pos_x]
        };
        if (result[1].T0[i].attributes.PdA !=0 && result[1].T0[i].attributes.PdB ==0 && result[1].T0[i].attributes.PdC ==0) {
          var colorcode = 1;
        }
        else if (result[1].T0[i].attributes.PdA ==0 && result[1].T0[i].attributes.PdB !=0 && result[1].T0[i].attributes.PdC ==0) {
          var colorcode = 1;
        }
        else if (result[1].T0[i].attributes.PdA ==0 && result[1].T0[i].attributes.PdB ==0 && result[1].T0[i].attributes.PdC !=0) {
          var colorcode = 1;
        }
        else if (result[1].T0[i].attributes.PdA !=0 && result[1].T0[i].attributes.PdB !=0 && result[1].T0[i].attributes.PdC ==0) {
          var colorcode = 2;
        }
        else if (result[1].T0[i].attributes.PdA !=0 && result[1].T0[i].attributes.PdB ==0 && result[1].T0[i].attributes.PdC !=0) {
          var colorcode = 2;
        }
        else if (result[1].T0[i].attributes.PdA ==0 && result[1].T0[i].attributes.PdB !=0 && result[1].T0[i].attributes.PdC !=0) {
          var colorcode = 2;
        }
        else if (result[1].T0[i].attributes.PdA !=0 && result[1].T0[i].attributes.PdB !=0 && result[1].T0[i].attributes.PdC !=0) {
          var colorcode = 3;
        }
        else {
          var colorcode = 0;
        }
        // var colorcode = (result[1].T0[i].attributes.PdA&1) + (result[1].T0[i].attributes.PdB&1) + (result[1].T0[i].attributes.PdC&1);
        feature['properties'] = {
          'ID': result[1].T0[i].v_id,
          'PA': result[1].T0[i].attributes.PdA,
          'QA': result[1].T0[i].attributes.QdA,
          'PB': result[1].T0[i].attributes.PdB,
          'QB': result[1].T0[i].attributes.QdB,
          'PC': result[1].T0[i].attributes.PdC,
          'QC': result[1].T0[i].attributes.QdC,
          'LOAD_MODEL': result[1].T0[i].attributes.Load_Model,
          'FLAG': result[1].T0[i].attributes.flag,
          'PHASE': result[1].T0[i].attributes.node_phases,
          'COLOR': colorcode
        }

        features.push(feature);
    }

    // var geoJson = new GeoJson();
    // console.log(features);
  	return {'type': type, 'features': features};
  }




  private dModleUrl = environment.gsqlPre + '/query/getres_D';  // URL to web api
  getresD(): Promise<resD[]> {
    return this.http.get(this.dModleUrl)
        .toPromise()
        .then(response => {
            const data = response.json().results;
            const res = [];
            console.log(data);
            for (const summ of data) {
                for(var i=0;i<summ['@@resI'].length;i++)
                    {
                      res.push({ 
                        'from': summ['@@resI'][i]['From_Node'],
                        'PVf':summ['@@resI'][i]['PV_flag'],
                        'batteryf':summ['@@resI'][i]['battery_flag'],
                        'windf':summ['@@resI'][i]['wind_flag']});
                    }

            }

            return res as resD[];
        })
}


}


export class resD {
fromNode:string;
PVf:string;
batteryf:string;
windf:string;

}

export interface IGeometry {
    type: string;
    coordinates: number[];
}

export interface IGeoJson {
    type: string;
    geometry: IGeometry;
    properties?: any;
}

export class GeoJson implements IGeoJson {
  type = 'Feature';
  geometry: IGeometry;
  properties: any;

  constructor(coordinates, properties, type) {
    this.geometry = {
      type: type,
      coordinates: coordinates
    }
    this.properties = properties;
  }
}
