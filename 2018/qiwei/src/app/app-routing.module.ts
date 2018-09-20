import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import {SeriesPowerFlowComponent} from './power-flow-cal/components/time-series/timeseries-powerflow.component';
import { TimeseriesVisComponent } from './g-data-vis/components/time-series/timeseries-vis.component';
import { LineTableComponent } from './g-data-vis/components/line/line.component';
import { RegulatorTableComponent } from './g-data-vis/components/regulator/regulator.component';
import { RegulatorResTableComponent } from './g-data-vis/components/regulatorRes/regulatorRes.component';
import { GeneratorTableComponent } from './g-data-vis/components/generator/generator.component';
import { CapacitorTableComponent } from './g-data-vis/components/capacitor/capacitor.component';
import { CapResultTableComponent } from './g-data-vis/components/capResult/capResult.component';
import { LineResultTableComponent } from './g-data-vis/components/lineResult/lineResult.component';
import { LoadDisplayTableComponent } from './g-data-vis/components/loadDisplay/loadDisplay.component';
import { WelcomeTableComponent } from './home/components/welcome/welcome.component';
import { SwitchTableComponent } from './g-data-vis/components/switch/switch/switch.component';
import { BusvoltageTableComponent } from './g-data-vis/components/busvoltage/busvoltage.component';
import { SummaryTableComponent } from './g-data-vis/components/summary/summary.component';
import { GraphManageComponent } from './app-shared/components/graph-manage/graph-manage/graph-manage.component';
import { DetailGraphComponent } from './detail-graph/components/detail-graph/detail-graph.component';
import { PowerFlowComponent } from './power-flow-cal/components/power-flow/power-flow.component';
import { DataCalculationComponent } from './data-analysis/components/data-calculation/data-calculation.component';
import { RetrieveCaseComponent } from './retrieve-case/components/retrieve-case/retrieve-case.component';
import { LoadForecastComponent } from './load-forecast/components/load-forecast/load-forecast.component';
import { LoaddataComponent } from './load-forecast/components/loaddata/loaddata.component';
import { PredictComponent } from './load-forecast/components/predict/predict.component';
import { IesloaddataComponent } from './load-forecast/components/iesloaddata/iesloaddata.component';
import { MainInterfaceComponent } from './IES/components/MainInterface/MainInterface.component';
import { BigDataComponent } from './data-analysis/components/big-data/big-data.component';
import { DataQualityComponent } from './data-analysis/components/data-quality/data-quality.component';
import { PrecisionMarketingComponent } from './data-analysis/components/precision-marketing/precision-marketing.component';
import { PrecisionMarketingUserComponent } from './data-analysis/components/precision-marketing-user/precision-marketing-user.component';
import { CrossValidComponent } from './data-analysis/components/cross-valid/cross-valid.component';
import { TimeVisComponent } from './data-analysis/components/time-vis/time-vis.component';
import { ClusteringComponent } from './data-analysis/components/clustering/clustering.component';
import { MovableLoadComponent } from './data-analysis/components/movable-load/movable-load.component';
import { GraphParentComponent } from './app-shared/components/chart/graph-parent/graph-parent.component';
// tslint:disable-next-line:max-line-length
import { GraphParentDataAnalysisComponent } from './app-shared/components/chart/graph-parent-data-analysis/graph-parent-data-analysis.component';
import { DqCompleteComponent } from './data-analysis/components/dq-complete/dq-complete.component'
import { AllTlComponent } from './data-analysis/components/all-tl/all-tl.component';
import { UserAnalysisComponent } from './data-analysis/components/user-analysis/user-analysis.component';
import {TopologyComponent} from './equip-manage/topology/topology.component';
import { BackLogComponent } from './app-shared/components/log-view/back-log/back-log.component';
import { AppLogComponent } from './app-shared/components/log-view/app-log/app-log.component';
import { UwsgiLogComponent } from './app-shared/components/log-view/uwsgi-log/uwsgi-log.component';
import { ReqLogComponent } from './app-shared/components/log-view/req-log/req-log.component';
import { LeafletmapComponent } from './app-shared/components/chart/leafletmap/leafletmap.component';
import { ModelingComponent } from './modeling/modeling.component';
import { LoadTableComponent } from './load-table/load-table.component';
import { DevQComponent } from './d-modeling/dev-q.component';
import { DAnalysisComponent } from './d-analysis/d-analysis.component';
import { EnetComponent } from './d-analysis/enet/enet.component';
import { HnetComponent } from './d-analysis/hnet/hnet.component';
import { TnetComponent } from './d-analysis/tnet/tnet.component';
import { MultiNetComponent } from './d-analysis/multi-net/multi-net.component';
import { DataPfComponent } from './d-modeling/data-pf/data-pf.component';
import {PfResultComponent} from './d-modeling/pf-result/pf-result.component';
import { NewclusteringComponent } from './newclustering/newclustering.component';
import { NewelasticityComponent } from './newelasticity/newelasticity.component';
import { FiltertypicalComponent } from './filtertypical/filtertypical.component';
import {FilterbusinessComponent } from './filterbusiness/filterbusiness.component';

const routes: Routes = [
  { path: '', redirectTo: '/view/welcome', pathMatch: 'full' },
  { path: 'welcome', component: WelcomeTableComponent },
  { path: 'modeling', component: ModelingComponent },
  { path: 'loadtable', component: LoadTableComponent },

  // { path: 'newprecision', component: NewclusteringComponent },
  { path: 'switch', component: SwitchTableComponent },
  { path: 'summary', component: SummaryTableComponent },
  { path: 'manage', component: GraphManageComponent},
  { path: 'detailgraph', component: DetailGraphComponent},
  { path: 'retrieve', component: RetrieveCaseComponent},
  { path: 'powerflow', component: PowerFlowComponent},
  { path: 'seriespowerflow', component: SeriesPowerFlowComponent},
  { path: 'datacalculation', component: DataCalculationComponent},
  { path: 'equipment', component: TopologyComponent},
  {path: 'd-modeling', component: DevQComponent},



{path: 'data-pf', component:DataPfComponent},
{path: 'pf-result', component:PfResultComponent},



{path:"Distribution",component: GraphParentComponent,
children: [
{path: 'd-analysis', component:DAnalysisComponent}]},

{ path: 'd-analysis',
component: GraphParentComponent,
children: [
{path: 'enet', component:EnetComponent},
{path: 'hnet', component:HnetComponent},
{path: 'tnet', component:TnetComponent},
{path: 'multi-net', component:MultiNetComponent}
]},




{ path: 'log',
component: BackLogComponent,
children: [
  { path: 'app', component: AppLogComponent},
  { path: 'req', component: ReqLogComponent},
  { path: 'uwsgi', component: UwsgiLogComponent}
]
},



  // the path 'modeling' to be modified
  { path: 'modeling',
  // component: GraphParentComponent,
  children: [
    {path: 'loadinput', component: ModelingComponent},
  ]
},
{ path: 'modeling',
component: GraphParentComponent,
children: [
  {path: 'loadtable', component: LoadTableComponent},
]
},

{ path: 'precision',
// component: GraphParentComponent,
children: [
  {path: 'filtertypical', component: FiltertypicalComponent},
]
},
{ path: 'precision',
// component: GraphParentComponent,
children: [
  {path: 'filterbusiness', component: FilterbusinessComponent},
]
},
{ path: 'precision',component: GraphParentComponent,
children: [
  {path: 'newelasticity', component: NewelasticityComponent},
]
},
{ path: 'precision',component: GraphParentComponent,
children: [
  {path: 'newclustering', component: NewclusteringComponent},
]
},
  { path: 'view',
    component: GraphParentComponent,
    children: [
      {path: 'timeseries-vis', component: TimeseriesVisComponent},
      {path: 'line', component: LineTableComponent},
      {path: 'capacitor', component: CapacitorTableComponent},
      { path: 'regulator', component: RegulatorTableComponent },
      { path: 'loadDisplay', component: LoadDisplayTableComponent },
      { path: 'generator', component: GeneratorTableComponent },
      { path: 'regulatorRes', component: RegulatorResTableComponent },
      { path: 'lineResult', component: LineResultTableComponent },
      { path: 'busvoltage', component: BusvoltageTableComponent },
      { path: 'capResult', component: CapResultTableComponent },
      { path: 'welcome', component: WelcomeTableComponent },
    ]
  },
  { path: 'data',
    component: GraphParentDataAnalysisComponent,
    children: [
      {path: 'useranalysis', component: UserAnalysisComponent},
      { path: 'bigdata', component: BigDataComponent},
      { path: 'dataquality', component: DataQualityComponent},
      { path: 'crossvalid', component: CrossValidComponent},
      { path: 'timevis', component: TimeVisComponent},
      { path: 'clustering/:id', component: ClusteringComponent},
      { path: 'movableload', component: MovableLoadComponent},
      { path: 'dqcomplete', component: DqCompleteComponent},
      { path: 'alltl', component: AllTlComponent},
      { path: 'precisionmarketing', component: PrecisionMarketingComponent},
      { path: 'precisionmarketinguser', component: PrecisionMarketingUserComponent},
      { path: 'forecast', 
        component: LoadForecastComponent,
        children: [
          {path: 'visualization', component: LoaddataComponent},
          {path: 'predict', component: PredictComponent}
          //{path: 'iesvisualization', component: IesloaddataComponent}
        ]},
    ]
  },
  { path: 'iesvisualization', component: IesloaddataComponent},
  //{ path: 'forecast', component: LoadForecastComponent,
  //  children: [
  //    {path: 'iesvisualization', component: IesloaddataComponent}]
  //},
  {path: 'leafletmap', component: LeafletmapComponent},
  {
    path: 'IES', 
    component: GraphParentComponent,  
    children: [{path:'main', component: MainInterfaceComponent}]
  }
];
@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
