import { TestBed, inject } from '@angular/core/testing';

import { NewclusteringService } from './newclustering.service';

describe('NewclusteringService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [NewclusteringService]
    });
  });

  it('should be created', inject([NewclusteringService], (service: NewclusteringService) => {
    expect(service).toBeTruthy();
  }));
});
