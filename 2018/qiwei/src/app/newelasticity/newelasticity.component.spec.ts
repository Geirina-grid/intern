import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { NewelasticityComponent } from './newelasticity.component';

describe('NewelasticityComponent', () => {
  let component: NewelasticityComponent;
  let fixture: ComponentFixture<NewelasticityComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ NewelasticityComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(NewelasticityComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
