import { TestBed, inject } from '@angular/core/testing';

import { ModleserService } from './modleser.service';

describe('ModleserService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [ModleserService]
    });
  });

  it('should be created', inject([ModleserService], (service: ModleserService) => {
    expect(service).toBeTruthy();
  }));
});
