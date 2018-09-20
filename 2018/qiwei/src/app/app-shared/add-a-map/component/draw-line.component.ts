import { Component, ViewChild, OnInit,EventEmitter,Input,Output } from '@angular/core';
import { DemodataService, GeoJson } from '../services/demodata.service'
import { WholeGraphService } from '../../services/whole-graph.service';
import * as mapboxgl from 'mapbox-gl';
import { datapfModule } from '../../../d-modeling/data-pf/data-pf.module';

@Component({
  selector: 'app-draw-line',
  templateUrl: './draw-line.component.html',
  styleUrls: ['./draw-line.component.css']
})
export class DrawLineComponent implements OnInit {
    @Output() featureO = new EventEmitter<string>();
    @Output() feature_lineO = new EventEmitter<string>();
    @Output() ppclick = new EventEmitter<string>();
    @ViewChild(DrawLineComponent) dlMap;
    public chart: any;
    private graph;
     
  constructor(private demodataService: DemodataService,
              private wholeGraphService: WholeGraphService) {
  	Object.getOwnPropertyDescriptor(mapboxgl, "accessToken").set(this.demodataService.mapbox_token);
  }

  // default settings
  map: mapboxgl.Map;
  style = 'mapbox://styles/mapbox/streets-v10';
  lat = 39.89909673439177;
  lng = 116.40036185208118;
  markflag=1;
  message = 'Hello World!';
  current_zoom = 11.2;
  circle_selected = false;
  Linklist:any;
  pointdata: any;
  Mwind:any;
  Mwind2:any;
  Mwind3:any;
  Mbattery:any;
  Mbattery2:any;
  Mbattery3:any;
  MPV:any;
  MPV2:any;
  MPV3:any;
  sumPV:string[]=[];
  sumwind:string[]=[];
  sumbattery:string[]=[];
  busID:string[]=[];
  count:any;
  popup = new mapboxgl.Popup({
    closeButton: false,
    closeOnClick: false
    });

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

    // this.addLine();
    this.addPoints();
    this.addLine();
    // console.log('map done');
    this.addPopup();


    // this.addLinePopup();

    this.map.on('load', function(){
        this.map.on('zoomend',function(){
            var zoomlevel=this.map.getZoom();
            console.log(zoomlevel);
            if(zoomlevel>12&&this.markflag==1)
            {
                this.demodataService.getresD()
                .then(data=>{
                    for(var j=0;j<this.pointdata["features"].length;j++)
                    {
                        this.busID.push(this.pointdata["features"][j].properties.ID)
                    }


                    for(var i=0;i<data.length;i++)
                    {
                        var node=data[i]['from'].slice(5,data[i]['from'].end)
                        var Idx=this.busID.indexOf(node)

                        var el = document.createElement('div');
                        // el.style.background= "url(\"https://i.imgur.com/MK4NUzI.png\")";
                        el.style.background= "url(\"https://imgur.com/q83GIMS.png\")" ;//wind G
                        el.style.height='70px';
                        el.style.width='70px';
                        if(data[i]['windf']=='ONLINE')
                        {
                            var Mmwind= new mapboxgl.Marker(el,{
                                offset: {
                                    x: -0,
                                    y: -30
                                }})
                            .setLngLat(this.pointdata["features"][Idx].geometry.coordinates)
                            .addTo(this.map);
                            this.sumwind.push(Mmwind);
                        }
                        if(data[i]['PVf']=='ONLINE')
                        {
                        var el = document.createElement('div');
                        el.style.background= "url(\"https://imgur.com/36GjuYH.png\")";//PV Panel
                        el.style.height='70px';
                        el.style.width='70px';
                        var MmPV= new mapboxgl.Marker(el,{
                            offset: {
                                x: -0,
                                y: -30
                            }})
                        .setLngLat(this.pointdata["features"][Idx].geometry.coordinates)
                        .addTo(this.map);
                        this.sumPV.push(MmPV);
                        }
                       
                        if(data[i]['batteryf']=='ONLINE')
                        {
                        var el = document.createElement('div');
                        el.style.background= "url(\"https://imgur.com/ujeWonT.png\")";//battery
                        el.style.height='70px';
                        el.style.width='70px';
                        var Mmbattery=new mapboxgl.Marker(el,{
                            offset: {
                                x: 0,
                                y: -30
                            }})
                        .setLngLat(this.pointdata["features"][Idx].geometry.coordinates)
                        .addTo(this.map);

                        this.sumbattery.push(Mmbattery);
                        }
                    this.markflag=0;
                    }

                         


                                
                        

                })


 
            }
            else if (zoomlevel>12&&this.markflag==0)
            {}
                else{
                    for(var k=0;k<this.sumwind.length;k++)
                    {
                        this.sumwind[k].remove();
                    }
                    for(var k=0;k<this.sumbattery.length;k++)
                    {
                        this.sumbattery[k].remove();
                    }
                    for(var k=0;k<this.sumPV.length;k++)
                    {
                        this.sumPV[k].remove();
                    }

                    this.markflag=1;
                }

            // if(zoomlevel>12&&this.markflag==1)
            // {
            //                         var el = document.createElement('div');
            //                         // el.style.background= "url(\"https://i.imgur.com/MK4NUzI.png\")";
            //                         el.style.background= "url(\"https://imgur.com/q83GIMS.png\")" ;//wind G
            //                         el.style.height='70px';
            //                         el.style.width='70px';
                    
            //                         this.Mwind= new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: -0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][0].geometry.coordinates)
            //                         .addTo(this.map);
                    
            //                         var el = document.createElement('div');
            //                         el.style.background= "url(\"https://imgur.com/36GjuYH.png\")";//PV Panel
            //                         el.style.height='70px';
            //                         el.style.width='70px';
            //                         this.MPV= new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: -0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][10].geometry.coordinates)
            //                         .addTo(this.map);
                            
                            
            //                         var el = document.createElement('div');
            //                         el.style.background= "url(\"https://imgur.com/ujeWonT.png\")";//battery
            //                         el.style.height='70px';
            //                         el.style.width='70px';
            //                         this.Mbattery=new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: 0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][20].geometry.coordinates)
            //                         .addTo(this.map);


            //                         var el = document.createElement('div');
            //                         el.style.background= "url(\"https://imgur.com/ujeWonT.png\")";//battery
            //                         el.style.height='70px';
            //                         el.style.width='70px';
            //                         this.Mbattery2=new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: 0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][30].geometry.coordinates)
            //                         .addTo(this.map);

            //                         var el = document.createElement('div');
            //                         el.style.background= "url(\"https://imgur.com/ujeWonT.png\")";//battery
            //                         el.style.height='70px';
            //                         el.style.width='70px';
            //                         this.Mbattery3=new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: 0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][40].geometry.coordinates)
            //                         .addTo(this.map);




            //                         var el = document.createElement('div');
            //                         el.style.background= "url(\"https://imgur.com/36GjuYH.png\")";//PV Panel
            //                         el.style.height='70px';
            //                         el.style.width='70px';
            //                         this.MPV2= new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: -0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][16].geometry.coordinates)
            //                         .addTo(this.map);
                            
            //                         var el = document.createElement('div');
            //                         el.style.background= "url(\"https://imgur.com/36GjuYH.png\")";//PV Panel
            //                         el.style.height='70px';
            //                         el.style.width='70px';
            //                         this.MPV3= new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: -0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][18].geometry.coordinates)
            //                         .addTo(this.map);
                            

            //                         var el = document.createElement('div');
            //                         // el.style.background= "url(\"https://i.imgur.com/MK4NUzI.png\")";
            //                         el.style.background= "url(\"https://imgur.com/q83GIMS.png\")" ;//wind G
            //                         el.style.height='70px';
            //                         el.style.width='70px';
                    
            //                         this.Mwind2= new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: -0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][35].geometry.coordinates)
            //                         .addTo(this.map);

            //                         var el = document.createElement('div');
            //                         // el.style.background= "url(\"https://i.imgur.com/MK4NUzI.png\")";
            //                         el.style.background= "url(\"https://imgur.com/q83GIMS.png\")" ;//wind G
            //                         el.style.height='70px';
            //                         el.style.width='70px';
                    
            //                         this.Mwind3= new mapboxgl.Marker(el,{
            //                             offset: {
            //                                 x: -0,
            //                                 y: -30
            //                             }})
            //                         .setLngLat(this.pointdata["features"][28].geometry.coordinates)
            //                         .addTo(this.map);
            //                     this.markflag=0;

            // }
        
            //  else if (zoomlevel>12&&this.markflag==0)
            // {}
            //     else{
            //         this.Mwind.remove();
            //         this.Mwind2.remove();
            //         this.Mwind3.remove();
            //         this.Mbattery.remove();
            //         this.Mbattery2.remove();
            //         this.Mbattery3.remove();
            //         this.MPV.remove();
            //         this.MPV2.remove();
            //         this.MPV3.remove();
            //         this.markflag=1;
            //     }



            

    
        }.bind(this));
    }.bind(this));
    
  }


  addLine() {

    this.map.on('load', function () {

        (<any>window).gsql = this.chart;

        this.wholeGraphService.getGraph()
        .then(graph => {
        this.graph = graph;
        // console.log(this.graph[1].T0[0].attributes.pos_x);
        this.Linklist = this.demodataService.LinkGenerator(this.graph);

        // console.log(this.Linklist);

        this.map.addLayer({
            "id": "route",
            "type": "line",
            "source": {
                "type": "geojson",
                "data": this.demodataService.getLineGeoJson(this.graph, this.pointdata)
            },
            "z-index": 2,
            "layout": {
                "line-join": "round",
                "line-cap": "round"
            },
            "paint": {
                // "line-color": "#888",
                "line-width": 2,
                // "line-color": "blue"
                // "fill-outline-color": "black",
                // "fill-outline-color": "rgba(0,0,0,0.0)",
                // "line-color": {
                //     property: 'CONFIG',
                //     stops: [
                //         [0, '#008C00'],
                //         [11, '#008C00'],
                //         [12, '#4096EE'],
                //         [20, '#4096EE']
                //     ]
                // }
                "line-color": ['get', 'COLOR']
            }
        });

        // var stateLegendEl = document.getElementById('load-legend');
      });     
    }.bind(this));
  }


  resizeMap() {
    setTimeout(()=>{
      this.dlMap.map.resize();
    }, 50);
  }



  
  addPoints() {
    this.map.on('load', function () {


        (<any>window).gsql = this.chart;
        this.wholeGraphService.getGraph()
            .then(graph => {
        this.graph = graph;
        this.pointdata = this.demodataService.getPointsGeoJson(this.graph);

        // console.log(this.pointdata);
        //this.resizeMap();


        var d1:JSON ;
        console.log(this.pointdata["features"][0]);
        console.log(d1);
 

        this.map.addLayer({
            "id": "points",
            "type": "circle",
            "source": {
                "type": "geojson",
                "data": this.pointdata
            },
            "z-index": 1,
            "paint": {
                "circle-radius": 5.5,
                // "circle-color": 'COLOR'
                "circle-color": {
                    property: 'COLOR',
                    stops: [
                        [0, '#60fff7'],
                        [1, 'red'],
                        [3, 'yellow']
                    ]
                }
            }
        });

    });
    }.bind(this));



  }

  addPopup() {
console.log(12)
    // this.map.on('mousemove', function (e) {
    //     var features = this.map.queryRenderedFeatures(e.point);
    //     var jsonstring = JSON.stringify(features[0].properties, null, 2);
    //     jsonstring = jsonstring.replace(/\n/g, ' ');
    //     jsonstring = jsonstring.slice(2,-2);
    //     document.getElementById('features').innerHTML = jsonstring;
    //     }.bind(this));

    this.map.on('click', function (e) {
        var features = this.map.queryRenderedFeatures(e.point);
        var jsonstring = JSON.stringify(features[0].properties, null, 2);
        jsonstring = jsonstring.replace(/\n/g, ' ');
        jsonstring = jsonstring.slice(2,-2);
        document.getElementById('features').innerHTML = jsonstring;

        this.featureO=features;
        this.ppclick.emit();

        }.bind(this));
          
        
    this.map.on('click', function(e) {

    (<any>window).gsql = this.chart;
        this.wholeGraphService.getGraph()
            .then(graph => {
        this.graph = graph;

        var features_pt = this.map.queryRenderedFeatures(e.point, {
            layers: ['points']
        });

        var features_line = this.map.queryRenderedFeatures(e.point, {
            layers: ['route']
            });

        // var feature_line = features_line[0];
        // console.log(feature_line.properties.CONFIG);
  
    if (!features_pt.length) {
      return;
    }
  
    // if (!features_line.length) {
    //     return;
    // }

    var feature = features_pt[0];
    var feature_line = features_line[0];

    //  this.featureO=feature;
    //  this.ppclick.emit();


 this.popup = new mapboxgl.Popup({ offset: [0, -15] })
      .setLngLat(feature.geometry.coordinates)
      .setHTML('<div class="sizer" style="width: 300px;"><div class="titlePane"><div class="titleButton prev hidden" title="Previous feature"></div><div class="titleButton next" title="Next feature"></div><div class="titleButton maximize" title="Maximize"></div><div class="titleButton close" title="Close"></div></div></div><div class="sizer content" style="width: 300px;"><div class="contentPane" style="max-height: 300px;"><div class="esriViewPopup" id="esri_dijit__PopupRenderer_7" widgetid="esri_dijit__PopupRenderer_7"><div class="statusSection hidden" dojoattachpoint="_status"></div><div class="mainSection"><div class="header">Beijing Distribution System</div><div class="hzLine"></div><div><table class="attrTable" cellpadding="0px" cellspacing="0px"><tbody><tr valign="top"><td class="attrName">Type</td><td class="attrValue">Bus</td></tr><tr valign="top"><td class="attrName">ID</td><td class="attrValue">' + feature.properties.ID + '</td></tr><tr valign="top"><td class="attrName">Latitude</td><td class="attrValue">' + feature.geometry.coordinates[1].toFixed(6) + '</td></tr><tr valign="top"><td class="attrName">Longitude</td><td class="attrValue">' + feature.geometry.coordinates[0].toFixed(6) + '</td></tr><tr valign="top"><td class="attrName">PA (W)</td><td class="attrValue"><span class="esriNumericValue">' + feature.properties.PA + '</span></td></tr><tr valign="top"><td class="attrName">QA (Var)</td><td class="attrValue">' + feature.properties.QA + '</td></tr><tr valign="top"><td class="attrName">PB (W)</td><td class="attrValue">' + feature.properties.PB + '</td></tr><tr valign="top"><td class="attrName">QB (Var)</td><td class="attrValue"><span class="esriDateValue">' + feature.properties.QB + '</span></td></tr><tr valign="top"><td class="attrName">PC (W)</td><td class="attrValue">' + feature.properties.PC + '</td></tr><tr valign="top"><td class="attrName">QC (Var)</td><td class="attrValue"><span class="esriDateValue">' + feature.properties.QC + '</span></td></tr><tr valign="top"><td class="attrName">Last Revised</td><td class="attrValue"><span class="esriDateValue">07/01/2018 5:00 pm</span></td></tr></tbody></table></div><div class="break"></div></div></div></div></div>')
      .addTo(this.map);
    });
  }.bind(this));
}

}