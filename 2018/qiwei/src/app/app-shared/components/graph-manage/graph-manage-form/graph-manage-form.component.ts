import { Component, Input, OnInit } from '@angular/core';
import { FormGroup } from '@angular/forms';

import { QuestionBase } from '../../../models/question-base';
import { QuestionControlService } from '../../../services/question-control.service';
import { WholeGraphService } from '../../../services/whole-graph.service';
import { EditGraphService } from '../../../services/edit-graph.service';

@Component({
  selector: 'app-graph-manage-form',
  templateUrl: './graph-manage-form.component.html',
  styleUrls: ['./graph-manage-form.component.css'],
  providers: [QuestionControlService]
})
export class GraphManageFormComponent implements OnInit {

  @Input() type;
  questions: QuestionBase<any>[] = [];
  form: FormGroup;
  // payLoad = '';

  constructor(
    private qcs: QuestionControlService,
    private editService: EditGraphService
  ) {
  }

  ngOnInit() {
    if (this.type === 'node') {
      this.editService.getNodeQuestions()
      .then(res => {
        this.questions = res;
        this.form = this.qcs.toFormGroup(res);
      });
    } else if (this.type === 'link') {
      this.editService.getLinkQuestions()
      .then(res => {
        this.questions = res;
        this.form = this.qcs.toFormGroup(res);
      })
    }
  }
}
