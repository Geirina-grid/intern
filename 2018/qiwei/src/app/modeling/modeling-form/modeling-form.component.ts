import { Component, Input, OnInit } from '@angular/core';
import { FormGroup } from '@angular/forms';

import { QuestionBase } from '../../app-shared/models/question-base';
import { QuestionControlService } from '../../app-shared/services/question-control.service';
import { WholeGraphService } from '../../app-shared/services/whole-graph.service';
import { EditGraphService } from '../../app-shared/services/edit-graph.service';

@Component({
  selector: 'app-modeling-form',
  templateUrl: './modeling-form.component.html',
  styleUrls: ['./modeling-form.component.css'],
  providers: [QuestionControlService]
})
export class ModelingFormComponent implements OnInit {

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
      this.editService.getLoadQuestions()
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
