import { TestBed, inject } from '@angular/core/testing';

import { FiltertypicalService } from './filtertypical.service';

describe('FiltertypicalService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [FiltertypicalService]
    });
  });

  it('should be created', inject([FiltertypicalService], (service: FiltertypicalService) => {
    expect(service).toBeTruthy();
  }));
});
