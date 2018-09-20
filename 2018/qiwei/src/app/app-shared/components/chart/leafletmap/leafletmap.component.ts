import { Component, OnInit, ViewChild, AfterViewInit } from '@angular/core';

import { icon, latLng, Layer, marker, circle, tileLayer } from 'leaflet';
import { HackMapDirective } from '../../../directives/hack-map.directive';
import { GraphChartService } from '../../../services/graph-chart.service';
import { LeafletMapLatLngService } from '../../../services/leafletmap.service';
import { GlobalIdService } from '../../../services/global-id.service';
import { Subscription } from 'rxjs/Subscription';
import * as L from 'leaflet';

@Component({
        selector: 'app-leafletmap',
        templateUrl: './leafletmap.component.html',
        providers: [LeafletMapLatLngService]
})
export class LeafletmapComponent implements OnInit, AfterViewInit {
        map: any;
        id: any;
        lat: number;
        lng: number;
        idsubscription: Subscription;
        @ViewChild(HackMapDirective) mapDirective: HackMapDirective;

        // Open Street Map definitions
        LAYER_OSM = tileLayer('http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom: 18, attribution: 'Open Street Map' });

        // Values to bind to Leaflet Directive
        options = {
                layers: [this.LAYER_OSM],
                zoom: 9,
                center: latLng(39.9042, 116.4074)
        };

        markers: Layer[] = [];
        // Added by Bowen
        constructor(
                private service: GraphChartService,
                private mapinfoService: LeafletMapLatLngService,
                private idService: GlobalIdService
        ) {
                this.idsubscription = this.idService.id$.subscribe(
                        (id) => {
                                console.log('new selected id arrived ' + id);
                                if (this.map !== undefined) {
                                        this.onMapUpdate(+id);
                                }
                        }
                )
        }

        ngOnInit() {
                this.generateMarker();
        }
        ngAfterViewInit() {
                this.map = this.mapDirective.map;
                (<any>window).map = this.map;
                this.announceMapInitiation();

        }
        generateMarker() {
                this.mapinfoService.GetMapLatLng().then(
                        result => {
                                // console.log(result);
                                length = result.length;
                                for (let i = 0; i < length; i++) {
                                        // console.log(length);
                                        this.id = result[i]['id'];
                                        this.lat = result[i]['detail']['lat'];
                                        this.lng = result[i]['detail']['lng'];

                                        if (this.id === 1) {
                                                const newCircle = circle([this.lat, this.lng],
                                                        {
                                                                name: this.id,
                                                                radius: 2000,
                                                                color: 'chocolate',
                                                                fillColor: 'chocolate',
                                                                opacity: 0.8,
                                                                fillOpacity: 0.8
                                                        }
                                                );
                                                // tslint:disable-next-line:max-line-length
                                                newCircle.bindTooltip('ID: ' + String(this.id) + ' Lat: ' + String(this.lat) + ', Lng: ' + String(this.lng));
                                                this.markers.push(newCircle);
                                        } else {
                                                const newCircle = circle([this.lat, this.lng],
                                                        {
                                                                name: this.id,
                                                                radius: 1000,
                                                                color: 'darkcyan',
                                                                fillColor: 'darkcyan',
                                                                opacity: 0.8,
                                                                fillOpacity: 0.8
                                                        }
                                                );
                                                // tslint:disable-next-line:max-line-length
                                                newCircle.bindTooltip('ID: ' + String(this.id) + ' Lat: ' + String(this.lat) + ', Lng: ' + String(this.lng));
                                                this.markers.push(newCircle);
                                        }
                                }

                        }
                )

                // for (let i = 0; i < 85; i++) {
                // const newMarker = marker(
                //        [ 46.879966 + 0.1 * (Math.random() - 0.5), -121.726909 + 0.1 * (Math.random() - 0.5) ],
                //                { icon: icon({
                //                   iconSize: [ 25, 41 ],
                //                   iconAnchor: [ 13, 41 ],
                //                   iconUrl: 'assets/marker-icon.png',
                //                   shadowUrl: 'assets/marker-shadow.png',
                //                  //opacity: 0.75
                //                })
                //          }
                //   );
                //      const newCircle = circle([39.9042 + 0.9 * (Math.random() - 0.5), 116.4074 + 0.9* (Math.random() - 0.5)],
                //                {
                //                        radius: 1000,
                //                        color: 'darkcyan',
                //                        fillColor: 'darkcyan',
                //                        opacity: 0.8,
                //                        fillOpacity: 0.8
                //                }
                //        );

                //this.markers.push(newMarker);
                //newCircle.bindTooltip("This is a test").openTooltip();
                //        newCircle.bindPopup("Hello new Circle");
                //        console.log(newCircle._latlng);
                //        this.markers.push(newCircle);
                //}
        }

        addMarker() {
                this.removeMarker();
                const newMarker = marker(
                        [39.9042 + 0.9 * (Math.random() - 0.5), 116.4074 + 0.9 * (Math.random() - 0.5)],
                        {
                                icon: icon({
                                        iconSize: [25, 41],
                                        iconAnchor: [13, 41],
                                        iconUrl: 'assets/marker-icon.png',
                                        shadowUrl: 'assets/marker-shadow.png'
                                })
                        }
                );
                //console.log(newMarker);
                this.markers.push(newMarker);
        }

        removeMarker() {
                this.markers.pop();
        }
        announceMapInitiation() {
                console.log('annouce Map initiation');
                console.log(this.map);
                this.service.announceMapInitiation(true);
                //                this.service.announceMap(this.map);
        }
        onMapUpdate(selectedmeter: number) {
                console.log('map update, new selected id ' + selectedmeter);
                console.log(this.map);
                let flag = 0;
                let latNew, lngNew, newlayer, newlayer2;
                this.map.eachLayer((layer) => {
                        // console.log(layer);
                        if (layer['options']['attribution'] !== 'Open Street Map') {
                                // console.log(layer);
                                if (layer['_latlng']) {
                                        // console.log(layer);
                                        if (layer['options']['name']) {
                                                if (layer['options']['name'] === selectedmeter) {
                                                        console.log('change flag ' + selectedmeter);
                                                        flag = 1;
                                                        latNew = layer['_latlng']['lat'];
                                                        lngNew = layer['_latlng']['lng'];
                                                        layer.remove();

                                                }
                                        }
                                }
                        }
                })
                if (flag === 1) {
                        this.map.eachLayer((layer) => {
                                if (layer['options']['color'] === 'chocolate') {
                                        this.id = layer['options']['name'];
                                        this.lat = layer['_latlng']['lat'];
                                        this.lng = layer['_latlng']['lng'];
                                        layer.remove();
                                }
                        })
                }
                if (flag === 1) {
                        newlayer = L.circle([latNew, lngNew], {
                                radius: 2000,
                                name: selectedmeter,
                                color: 'chocolate',
                                fillColor: 'chocolate',
                                opacity: 0.8,
                                fillOpacity: 0.8
                        }).addTo(this.map);
                        newlayer.bindTooltip('ID: ' + String(selectedmeter) + ' Lat: ' + String(latNew) + ', Lng: ' + String(lngNew));
                        this.map.addLayer(newlayer);

                        newlayer2 = L.circle([this.lat, this.lng], {
                                radius: 1000,
                                name: selectedmeter,
                                color: 'darkcyan',
                                fillColor: 'darkcyan',
                                opacity: 0.8,
                                fillOpacity: 0.8
                        }).addTo(this.map);
                        newlayer2.bindTooltip('ID: ' + String(selectedmeter) + ' Lat: ' + String(this.lat) + ', Lng: ' + String(this.lng));
                        this.map.addLayer(newlayer2);
                }
        }
}

