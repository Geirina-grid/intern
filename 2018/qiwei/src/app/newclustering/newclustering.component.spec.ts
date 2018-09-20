import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { NewclusteringComponent } from './newclustering.component';

describe('NewclusteringComponent', () => {
  let component: NewclusteringComponent;
  let fixture: ComponentFixture<NewclusteringComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ NewclusteringComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(NewclusteringComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
