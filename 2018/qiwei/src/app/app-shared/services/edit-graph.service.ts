import { Injectable } from '@angular/core';

import { QuestionDropdown } from '../models/question-dropdown';
import { QuestionBase } from '../models/question-base';
import { QuestionTextbox } from '../models/question-textbox';
import { WholeGraphService } from './whole-graph.service';

@Injectable()
export class EditGraphService {

  constructor(
    private graphService: WholeGraphService
  ) { }

  getLoadQuestions(): Promise<any> {
    let res;
    res = this.graphService.getSchema()
      .then(resp => {
        // let order = 3;
        let required = false;
        let questions: QuestionBase<any>[] = [
          new QuestionDropdown({
            key: 'type',
            label: 'Node Type',
            options: [
              { key: 'bus_D', value: 'bus_D' }
            ],
            required: true,
            order: 0
          }),
          new QuestionTextbox({
            key: 'exId',
            label: 'exId',
            value: '',
            required: true,
            order: 1
          }),
          new QuestionTextbox({
            key: 'PdA',
            label: 'PdA',
            value: '',
            required: true,
            order: 2
          }),
          new QuestionTextbox({
            key: 'QdA',
            label: 'QdA',
            value: '',
            required: true,
            order: 3
          }),
          new QuestionTextbox({
            key: 'PdB',
            label: 'PdB',
            value: '',
            required: true,
            order: 4
          }),
          new QuestionTextbox({
            key: 'QdB',
            label: 'QdB',
            value: '',
            required: true,
            order: 5
          }),
          new QuestionTextbox({
            key: 'PdC',
            label: 'PdC',
            value: '',
            required: true,
            order: 6
          }),
          new QuestionTextbox({
            key: 'QdC',
            label: 'QdC',
            value: '',
            required: true,
            order: 7
          }),
          new QuestionTextbox({
            key: 'Load_Model',
            label: 'Load_Model',
            value: '',
            required: true,
            order: 8
          }),
        ];
        // resp.VertexTypes.forEach(t => {
        //   if (t.Name === 'bus_D') {
        //     t.Attributes.forEach(a => {
        //       if (a.AttributeName === 'exId') {
        //         required = true;
        //       } 
        //       else {
        //         required = false;
        //       }
        //       // questions.push(new QuestionTextbox({
        //       //   key: a.AttributeName,
        //       //   label: a.AttributeName,
        //       //   value: '',
        //       //   required: required,
        //       //   order: order
        //       // }))
        //       ;
        //       order += 1;
        //     });
        //   }
        // }
      // );
        return questions.sort((a, b) => a.order - b.order);
      });
    return res;
  }

  getNodeQuestions(): Promise<any> {
    let res;
    res = this.graphService.getSchema()
      .then(resp => {
        let order = 1;
        let required = false;
        let questions: QuestionBase<any>[] = [
          new QuestionDropdown({
            key: 'type',
            label: 'Node Type',
            options: [
              { key: 'bus_D', value: 'bus_D' }
            ],
            required: true,
            order: 0
          })
        ];
        resp.VertexTypes.forEach(t => {
          if (t.Name === 'bus_D') {
            t.Attributes.forEach(a => {
              if (a.AttributeName === 'exId') {
                required = true;
              } 
              else {
                required = false;
              }
              questions.push(new QuestionTextbox({
                key: a.AttributeName,
                label: a.AttributeName,
                value: '',
                required: required,
                order: order
              }));
              order += 1;
            });
          }
        });
        return questions.sort((a, b) => a.order - b.order);
      });
    return res;
  }

  getLinkQuestions() {
    let res;
    res = this.graphService.getSchema()
      .then(resp => {
        let order = 5;
        let required = false;
        let questions: QuestionBase<any>[] = [
          new QuestionDropdown({
            key: 'etype',
            label: 'Edge Type',
            options: [
              { key: 'backe_D', value: 'backe_D' },
              { key: 'foree_D', value: 'foree_D' }
            ],
            required: true,
            order: 0
          }),

          new QuestionDropdown({
            key: 'stype',
            label: 'Source Type',
            options: [
              { key: 'bus_D', value: 'bus_D' }
            ],
            required: true,
            order: 1
          }),

          new QuestionTextbox({
            key: 'sid',
            label: 'Source ID',
            value: '',
            required: true,
            order: 2
          }),

          new QuestionDropdown({
            key: 'ttype',
            label: 'Target Type',
            options: [
              { key: 'bus_D', value: 'bus_D' }
            ],
            required: true,
            order: 3
          }),

          new QuestionTextbox({
            key: 'tid',
            label: 'Target ID',
            value: '',
            required: true,
            order: 4
          })];
        resp.EdgeTypes.forEach(t => {
          if (t.Name === 'foree_D') {
            t.Attributes.forEach(a => {
              if (a.AttributeName === 'edge_name') {
                required = true;
              } else {
                required = false;
              }
              questions.push(new QuestionTextbox({
                key: a.AttributeName,
                label: a.AttributeName,
                value: '',
                required: required,
                order: order
              }));
              order += 1;
            });
          }
        });
        return questions.sort((a, b) => a.order - b.order);
      });
    return res;
  }
}
