import { Component, OnInit } from '@angular/core';
import { DemodataService } from '../services/demodata.service'

import * as mapboxgl from 'mapbox-gl';

@Component({
  selector: 'app-add-a-map',
  templateUrl: './add-a-map.component.html',
  styleUrls: ['./add-a-map.component.css']
})
export class AddAMapComponent implements OnInit {

  constructor(private demodataService: DemodataService) {
  	Object.getOwnPropertyDescriptor(mapboxgl, "accessToken").set(this.demodataService.mapbox_token);
  }

  // default settings
  map: mapboxgl.Map;
  style = 'mapbox://styles/mapbox/streets-v10';
  lat = 39.90640180881081;
  lng = 116.39684467353356;
  message = 'Hello World!';
  current_zoom = 11;
  circle_selected = false;

  ngOnInit() {
  	this.showMap();
  }

  showMap() {
    this.map = new mapboxgl.Map({
      container: 'map1',
      style: this.style,
      zoom: this.current_zoom,
      center: [this.lng, this.lat]
    });

  }

}