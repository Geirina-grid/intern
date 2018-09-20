import { TestBed, inject } from '@angular/core/testing';

import { LoaddatagraphService } from './loaddatagraph.service';

describe('LoaddatagraphService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [LoaddatagraphService]
    });
  });

  it('should be created', inject([LoaddatagraphService], (service: LoaddatagraphService) => {
    expect(service).toBeTruthy();
  }));
});
