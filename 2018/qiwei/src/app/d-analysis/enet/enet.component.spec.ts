import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { EnetComponent } from './enet.component';

describe('EnetComponent', () => {
  let component: EnetComponent;
  let fixture: ComponentFixture<EnetComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ EnetComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(EnetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
