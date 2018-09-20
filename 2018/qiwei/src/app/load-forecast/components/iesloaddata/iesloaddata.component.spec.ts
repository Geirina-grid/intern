import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { IesloaddataComponent } from './iesloaddata.component';

describe('IesloaddataComponent', () => {
  let component: IesloaddataComponent;
  let fixture: ComponentFixture<IesloaddataComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ IesloaddataComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(IesloaddataComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
