import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DAnalysisComponent } from './d-analysis.component';

describe('DAnalysisComponent', () => {
  let component: DAnalysisComponent;
  let fixture: ComponentFixture<DAnalysisComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DAnalysisComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DAnalysisComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
