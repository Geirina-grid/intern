import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { NewtypicalComponent } from './newtypical.component';

describe('NewtypicalComponent', () => {
  let component: NewtypicalComponent;
  let fixture: ComponentFixture<NewtypicalComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ NewtypicalComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(NewtypicalComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
