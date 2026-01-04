### stackoverflow_unix_timestamps_integers.txt
- **Source:** Stack Overflow answers-to-questions temporal network from SNAP (Stanford Network Analysis Project)
- **URL:** https://snap.stanford.edu/data/sx-stackoverflow-a2q.txt.gz
- **Domain:** System timestamps / temporal networks
- **Digit Length:** 10 digits (homogeneous: 100%)
- **Count:** 17,823,525 integers
- **Range:** Unix timestamps from 2008-2015 (approximately 1220000000 to 1430000000)
- **Description:** Unix timestamps (seconds since epoch) extracted from Stack Overflow user interaction events
- **Date Downloaded:** January 2026
- **License:** Available for research use (SNAP datasets)

### cit_patents_citing_integers.txt
- **Source:** US Patent citation network 1975-1999 from SNAP (Stanford Network Analysis Project)
- **URL:** https://snap.stanford.edu/data/cit-Patents.txt.gz
- **Domain:** Patent system / scientific citations
- **Digit Length:** 7 digits (homogeneous: 100%)
- **Count:** 16,518,948 integers
- **Range:** US patent numbers 3,858,241 to 5,999,999
- **Description:** Citing patent IDs from US patent citation network; represents patents granted between 1975-1999
- **Date Downloaded:** January 2026
- **License:** Available for research use (SNAP datasets, original data from NBER)

### twitter.json
- **Source:** Export of data from Twitter's API
- **Domain:** Social media data
- **Digit Length Distribution:** Heterogeneous:
  - 1 digit: 28.37%
  - 2 digits: 17.12%
  - 3 digits: 23.82%
  - 4 digits: 4.98%
  - 5 digits: 2.80%
  - 6 digits: 0.33%
  - 8 digits: 0.47%
  - 9 digits: 1.95%
  - 10 digits: 10.67%
  - 18 digits: 9.35%
  - 20 digits: 0.14%
- **Count:** 2,108 integers
- **Description:** JSON file commonly used for benchmarking; integers extracted from Twitter API data; exhibits heterogeneous distribution with significant proportions of small integers (principally 1–3 digits) as well as larger integers (10 and 18 digits)

### citm_catalog.json
- **Source:** Catalog of events that occurred in a venue part of CitM (Cité de la Musique) in Paris
- **Domain:** Cultural event catalog
- **Digit Length Distribution:** Homogeneous:
  - 5 digits: 5.92%
  - 6 digits: 0.38%
  - 9 digits: 92.02%
  - 13 digits: 1.69%
- **Count:** 14,392 integers
- **Description:** JSON file commonly used for benchmarking; more homogeneous distribution with 92% of the integers having 9 decimal digits
