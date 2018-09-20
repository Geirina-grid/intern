import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ModelingFormComponent } from './modeling-form.component';

describe('ModelingFormComponent', () => {
  let component: ModelingFormComponent;
  let fixture: ComponentFixture<ModelingFormComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ModelingFormComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ModelingFormComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
