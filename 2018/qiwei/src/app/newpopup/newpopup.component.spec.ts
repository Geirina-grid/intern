import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { NewpopupComponent } from './newpopup.component';

describe('NewpopupComponent', () => {
  let component: NewpopupComponent;
  let fixture: ComponentFixture<NewpopupComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ NewpopupComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(NewpopupComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
