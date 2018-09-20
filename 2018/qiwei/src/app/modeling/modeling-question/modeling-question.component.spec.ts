import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ModelingQuestionComponent } from './modeling-question.component';

describe('ModelingQuestionComponent', () => {
  let component: ModelingQuestionComponent;
  let fixture: ComponentFixture<ModelingQuestionComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ModelingQuestionComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ModelingQuestionComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
