import { TestBed, inject } from '@angular/core/testing';

import { NewelasticityService } from './newelasticity.service';

describe('NewelasticityService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [NewelasticityService]
    });
  });

  it('should be created', inject([NewelasticityService], (service: NewelasticityService) => {
    expect(service).toBeTruthy();
  }));
});
