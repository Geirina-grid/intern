import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { HnetComponent } from './hnet.component';

describe('HnetComponent', () => {
  let component: HnetComponent;
  let fixture: ComponentFixture<HnetComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ HnetComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(HnetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
