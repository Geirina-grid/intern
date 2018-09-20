import { Injectable } from '@angular/core';

import { Observable } from 'rxjs/Observable';
import { of } from 'rxjs/observable/of';
import { Http, Response } from '@angular/http';
import 'rxjs/add/operator/map';


import { environment } from '../../../environments/environment';



@Injectable()
export class FiltertypicalService {

  constructor(private http: Http) { }

  product_data: Product[] = [];

  map_features: GeoJson[] = [];

  sites: Site[] = [];

  getData(): Observable<Product[]> {
    return of(this.product_data);
  }

  getGeoJson(zoom): Observable<GeoJson[]> {
    this.map_features = [];
    if (zoom >= 3) {
      for (var i=0; i<this.sites.length; i++) {
        var geo_json = new GeoJson([this.sites[i]['country'][1],this.sites[i]['country'][0]], { "museum_count": this.sites[i]['quantity'] });
        this.map_features.push(geo_json);
      }
    }
    else {
      for (var i=0; i<this.sites.length; i++) {
        var coord = this.sites[i]['country'];
        var new_coord = this.getCoord(coord[0], coord[1]);
        var index = this.getIndex(new_coord[1], new_coord[0]);
        if (index == -1){
          var geo_json = new GeoJson([new_coord[1], new_coord[0]], { "museum_count": this.sites[i]['quantity'] });
          this.map_features.push(geo_json);
        } else {
          this.map_features[index]['properties']['museum_count'] += this.sites[i]['quantity'];
        }
      }
    }
    return of(this.map_features);
  }

  processData(data){
    console.log(data);
    this.product_data = [];
    this.sites = [];

    var temp = data['results'][0]['Invens'];
    for(var i=0; i<temp.length; i++) {
      var id = temp[i]['attributes']['Invens.@productname'];
      var lat = temp[i]['attributes']['Invens.@latitude'];
      var lng = temp[i]['attributes']['Invens.@longitude'];
      var num = temp[i]['attributes']['Invens.quantity'];
      var cate = temp[i]['v_id'].split(':')[3];
      if (cate === 'FGI'){
        cate = 'Brand';
      }
      else {
        cate = 'Commodity Group';
      }

      this.product_data.push({name: id, category: cate, quantity: num, onhand: num});

      var site_index = this.getIndexInSites(lat, lng);
      if(site_index == -1) {
        this.sites.push({country: [lat, lng], quantity: num});
      } else {
        this.sites[site_index]['quantity'] += num;
      }
    }

  }

  getIndexInSites(lat, lng): any {
    for (var i=0; i<this.sites.length; i++) {
      if (lat == this.sites[i]['country'][0] && lng == this.sites[i]['country'][1]){
        return i;
      }
    }
    return -1;
  }

  getCoord(lat, lng): number[] {
    for (var i=0; i<REGION_MAPPING.length; i++) {
      if(lat == REGION_MAPPING[i]['country'][0] && lng == REGION_MAPPING[i]['country'][1]){
        return REGION_MAPPING[i]['area'];
      }
    }
    return [];
  }

  getIndex(lat, lng): any {
    for(var i=0; i<this.map_features.length; i++) {
      if(this.map_features[i]['geometry']['coordinates'][0] == lng && this.map_features[i]['geometry']['coordinates'][1] == lat) {
        //this.map_features[i]['properties']['museum_count']
        return i;
      }
    }
    return -1;
  }

  getURL(params): String {
    let gsqlPre = environment.gsqlPre;
    //var url = gsqlPre + "/query/product_graph/getInfoMod?" + params.join("&");
    var url = "http://localhost:3000/query/product_graph/getInfoMod?" + params;
    console.log(url);
    return url;
  }

  loadData(url): Observable<String> {
    return this.http.get(url).map((result: Response) => result.json());

  	// $.ajax({
    //     url: url,
    //     contentType: "application/json; charset=UTF-8",
    //     success: function(data){
    //       this.text = data;
    //       console.log(data);
    //       return data;
    //       // TODO
    //     }.bind(this),
    //     error: function (XMLHttpRequest, textStatus, errorThrown) {
    //         // status code
    //         console.log(XMLHttpRequest.status);
    //         // status
    //         console.log(XMLHttpRequest.readyState);
    //         // error message   
    //         console.log(textStatus);
    //     }.bind(this)
    // });
  }

  
}

export interface Product {
  name: string;
  category: string;
  quantity: number;
  onhand: number;
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

  constructor(coordinates, properties) {
    this.geometry = {
      type: 'Point',
      coordinates: coordinates
    }
    this.properties = properties;
  }
}

export interface Site {
  country: number[],
  quantity: number,
}
//lat,lng
export interface Region {
  country: number[];
  area: number[];
}

const REGION_MAPPING: Region[] = [
  {country: [48.85953, 2.32629], area: [45.580328, 6.209634]},
  {country: [48.21816, 11.62654], area: [45.580328, 6.209634]},
  {country: [41.93358, 12.45293], area: [45.580328, 6.209634]},
  {country: [19.30455, -99.14993], area: [41.768722, -105.05907]},
  {country: [32.74652, -97.09739], area: [41.768722, -105.05907]},
  {country: [43.6438, -79.37826], area: [41.768722, -105.05907]}
]
// export interface Name {
//   id: String,
//   name: String
// }

// const PRODUCTMAPPING: Name[] = [
//   {id:'AA', name:'Heat sink'},
//   {id:'AB', name:'Bezel'},
//   {id:'AC', name:'Planar'},
//   {id:'AD', name:'HDD'},
//   {id:'AE', name:'Power Supply'},
//   {id:'AF', name:'Memory'},
//   {id:'AG', name:'Lorem Semper Auctor'},
//   {id:'AH', name:'Ultrices Mauris'},
//   {id:'AI', name:'Tortor Dictum'},
//   {id:'AJ', name:'Suscipit'},
//   {id:'AK', name:'Fringilla'},
//   {id:'AL', name:'Pellentesque'}
// ];