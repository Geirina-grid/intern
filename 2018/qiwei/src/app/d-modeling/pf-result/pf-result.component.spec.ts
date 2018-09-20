import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { PfResultComponent } from './pf-result.component';

describe('PfResultComponent', () => {
  let component: PfResultComponent;
  let fixture: ComponentFixture<PfResultComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ PfResultComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(PfResultComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
