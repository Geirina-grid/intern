import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';
import { QuestionDropdown } from '../../app-shared/models/question-dropdown';
import { QuestionBase } from '../../app-shared/models/question-base';
import { QuestionTextbox } from '../../app-shared/models/question-textbox';
import { Switch } from '../models/switch';
import { ChangeSW } from '../models/ChangeSW';


@Injectable()
export class SwitchService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
    private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    private graphswitchUrl = environment.gsqlPre + '/query/graph_switchvis_D';
    private switchUrl = environment.gsqlPre + '/query/switchstatus_D';
    private changedswUrl = environment.gsqlPre + '/query/reconfiguration_D';
    private origingraphUrl = environment.gsqlPre + '/graph';
    private SdirectUrl = environment.gsqlPre + '/query/switchDirection_D';
    constructor(private http: Http) { }

    switchDirect(params): Promise<any> {
        let suffix = '?deleteswitch=' + params.switchdel + '&addswitch=' + params.switchadd + '&addswitch_F=' + params.addswitchF + '&addswitch_T=' + params.addswitchT;
        // console.log(this.graphUrl + suffix);
        return this.http.get(this.SdirectUrl + suffix)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                res.push(data[0]['direction']);
                res.push(data[1]['@@Edgeindex']);
                res.push(data[2]['@@Edgelist']);
                res.push(data[3]['@@Nodelist']);
                console.log(res);
                return res;

            })
            .catch(this.handleError);
    }




    getSwitches(): Promise<Switch[]> {
        return this.http.get(this.switchUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const sw of data) {
                    res.push({
                        'switch_name': sw['x.switch_name'],
                        'from_bus': sw['x.from_bus'],
                        'to_bus': sw['x.to_bus'],
                        'status': sw['x.status'],
                        'startnode': sw['x.startnode'],
                    });
                }
                console.log(res);
                return res as Switch[];
            })
            .catch(this.handleError);
    }

    getchangedsw(): Promise<ChangeSW[]> {
        return this.http.get(this.changedswUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(response);
                console.log(data);
                for (const csw of data) {
                    res.push({
                        'id': csw['@@id'],
                        'swtoopen': csw['k.swtoopen'],
                        'swtoclose': csw['k.swtoclose'],
                    });
                }
                console.log(res);
                return res as ChangeSW[];
            })
            .catch(this.handleError);
    }

    getGraph(): Promise<any> {
        return this.http.get(this.graphUrl)
            .toPromise()
            .then(response => response.json().results)
            .catch(this.handleError);
    }
    getGraphswitch(): Promise<any> {
        return this.http.get(this.graphswitchUrl)
            .toPromise()
            .then(response => response.json().results)
            .catch(this.handleError);
    }


    // this function called to post any change of graph, include add/delete/update a node/link
    postGraph(payload: any): Promise<any> {
        return this.http.post(this.origingraphUrl, payload, { headers: this.headers })
            .toPromise()
            .then(response => response.json())
            .catch(this.handleError);
    }

    // this function called to delete a node
    deleteNode(type: string, id: string): Promise<any> {
        const deleteUrl = this.origingraphUrl + '/vertices/' + type + '/' + id;
        return this.http.delete(deleteUrl)
            .toPromise()
            .then(resp => resp.json())
            .catch(this.handleError);
    }
    // this function called to delete a link
    // this function called to delete a link

    deleteLink(sType: string, sId: string, eType: string, tType: string, tId: string): Promise<any> {
        const deleteUrl = this.origingraphUrl + '/edges/' + sType + '/' + sId + '/' + eType + '/' + tType + '/' + tId;
        return this.http.delete(deleteUrl)
            .toPromise()
            .then(resp => resp.json())
            .catch(this.handleError);
    }
    getNodeQuestions() {
        const questions: QuestionBase<any>[] = [

            new QuestionDropdown({
                key: 'switch_status',
                label: 'Status',
                options: [
                    { key: 'open', value: 'open' },
                    { key: 'closed', value: 'closed' }
                ],
                required: true,
                order: 0
            }),


        ];

        return questions.sort((a, b) => a.order - b.order);
    }


    private handleError(error: any): Promise<any> {
        console.error('An error occurred', error); // for demo purposes only
        return Promise.reject(error.message || error);
    }
}
