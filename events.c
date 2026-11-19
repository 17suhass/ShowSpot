#include "project.h"

struct Event *head = NULL;
int eventIdCounter = 10;

const char *getCategoryName(int category) {
    switch (category) {
        case 1:
            return "Commercial Movie";
        case 2:
            return "Regional Theatre/Drama";
        case 3:
            return "Local Music Performance";
        case 4:
            return "Folk & Traditional Dance";
        case 5:
            return "Cultural Event";
        default:
            return "General Event";
    }
}

int hasAvailableSeats(struct Event *ptr) {
    int i, j;
    if (ptr == NULL) {
        return 0;
    }
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 12; j++) {
            if (ptr->seats[i][j] != 'X') {
                return 1;
            }
        }
    }
    return 0;
}

void addEvent(int id, char title[], int category, char artist[], char venue[], char time[], float regularPrice, float vipPrice, int isApproved) {
    struct Event *newnode = (struct Event *)malloc(sizeof(struct Event));
    int i, j;

    if (newnode == NULL) {
        printf("\nMemory allocation failed for new event!\n");
        return;
    }

    newnode->id = id;
    strcpy(newnode->title, title);
    newnode->category = category;
    strcpy(newnode->artist, artist);
    strcpy(newnode->venue, venue);
    strcpy(newnode->time, time);
    newnode->regularPrice = regularPrice;
    newnode->vipPrice = vipPrice;
    newnode->isApproved = isApproved;
    newnode->waitlistFront = NULL;
    newnode->waitlistRear = NULL;

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 12; j++) {
            if (i < 2) {
                newnode->seats[i][j] = 'V';
            } else {
                newnode->seats[i][j] = 'O';
            }
        }
    }
    newnode->next = NULL;

    if (head == NULL) {
        head = newnode;
    } else {
        struct Event *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newnode;
    }
}

void initDefaultEvents(void) {
    int i, j;
    struct Event *e6;
    struct Booking *bInit;

    addEvent(1, "Inception (Re-Release)", 1, "Warner Bros Pictures", "PVR Cinemas, Screen 2", "02:30 PM", 250.00, 400.00, 1);
    addEvent(2, "Kantara: Cultural Legend", 1, "Hombale Films", "Cinepolis Multiplex", "06:00 PM", 200.00, 350.00, 1);
    addEvent(3, "Natsamrat (Classic Marathi Drama)", 2, "Sahyadri Natya Troupe", "Bal Gandharva Rangmandir", "07:00 PM", 150.00, 250.00, 1);
    addEvent(4, "Baul & Sufi Folk Music Night", 3, "Kiran Baul & Troupe", "Kalagram Open Air Theatre", "05:30 PM", 100.00, 180.00, 1);
    addEvent(5, "Ghoomar & Kalbeliya Folk Dance", 4, "Rajasthan Heritage Artists", "Town Hall Cultural Stage", "08:00 PM", 120.00, 200.00, 1);

    addEvent(6, "Sold-Out Cultural Gala (Waitlist Demo Fixture)", 5, "Heritage Cultural Troupe", "City Grand Amphitheatre", "07:00 PM", 150.00, 250.00, 1);

    e6 = head;
    while (e6 != NULL && e6->id != 6) {
        e6 = e6->next;
    }

    if (e6 != NULL) {
        for (i = 0; i < 10; i++) {
            for (j = 0; j < 12; j++) {
                e6->seats[i][j] = 'X';
            }
        }

        bInit = (struct Booking *)malloc(sizeof(struct Booking));
        if (bInit != NULL) {
            bInit->bookingId = bookingCounter++;
            strcpy(bInit->buyerName, "Demo Seat Holder (Test Fixture)");
            bInit->eventId = 6;
            strcpy(bInit->eventTitle, e6->title);
            bInit->category = e6->category;
            bInit->row = 'A';
            bInit->col = 1;
            bInit->price = e6->vipPrice;
            bInit->next = NULL;

            if (bookingHead == NULL) {
                bookingHead = bInit;
            } else {
                struct Booking *bTemp = bookingHead;
                while (bTemp->next != NULL) {
                    bTemp = bTemp->next;
                }
                bTemp->next = bInit;
            }
        }
    }
}

void displayEvents(int filterMode) {
    struct Event *ptr = head;
    int count = 0;

    if (ptr == NULL) {
        printf("\nNo events registered in the system.\n");
        return;
    }

    printf("\n========================================================================================================================\n");
    if (filterMode == 1) {
        printf("                                             COMMERCIAL ENTERTAINMENT SHOWS                                             \n");
    } else if (filterMode == 2) {
        printf("                                    REGIONAL & LOCAL ARTIST ENTERTAINMENT SHOWS                                         \n");
    } else {
        printf("                                          ALL AVAILABLE SHOWS & PERFORMANCES                                            \n");
    }
    printf("========================================================================================================================\n");
    printf("%-4s %-36s %-25s %-20s %-10s %-10s %-10s\n", "ID", "Title", "Category", "Artist / Production", "Time", "Regular", "VIP");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    while (ptr != NULL) {
        int shouldShow = 0;
        if (filterMode == 0) {
            shouldShow = 1;
        } else if (filterMode == 1 && ptr->category == 1) {
            shouldShow = 1;
        } else if (filterMode == 2 && ptr->category >= 2) {
            shouldShow = 1;
        }

        if (shouldShow && ptr->isApproved == 1) {
            printf("%-4d %-36s %-25s %-20s %-10s Rs. %-6.2f Rs. %-6.2f\n",
                   ptr->id, ptr->title, getCategoryName(ptr->category),
                   ptr->artist, ptr->time, ptr->regularPrice, ptr->vipPrice);
            count++;
        }
        ptr = ptr->next;
    }

    if (count == 0) {
        printf("No matching events found for this selection.\n");
    }
    printf("========================================================================================================================\n");
}

void displaySeats(struct Event *ptr) {
    int i, j;
    printf("\n----------------------------------------------------------------------\n");
    printf(" Seating Arrangement for: %s\n", ptr->title);
    printf(" Venue: %s | Time: %s\n", ptr->venue, ptr->time);
    printf(" Pricing -> VIP (Rows A-B): Rs. %.2f | Regular (Rows C-J): Rs. %.2f\n", ptr->vipPrice, ptr->regularPrice);
    printf("----------------------------------------------------------------------\n");
    printf("      ");
    for (j = 1; j <= 12; j++) {
        printf("%2d ", j);
    }
    printf("\n     -------------------------------------\n");

    for (i = 0; i < 10; i++) {
        printf(" %c |  ", 'A' + i);
        for (j = 0; j < 12; j++) {
            printf(" %c ", ptr->seats[i][j]);
        }
        if (i < 2) {
            printf("  [VIP]");
        } else {
            printf("  [Regular]");
        }
        printf("\n");
        if (i == 1) {
            printf("     -------------------------------------\n");
        }
    }
    printf("     -------------------------------------\n");
    printf("Legend: [V] = Available VIP  |  [O] = Available Regular  |  [X] = Booked\n");
}

void viewSeatLayout(void) {
    int eventId;
    struct Event *ptr = head;

    displayEvents(0);
    if (head == NULL) {
        return;
    }

    printf("\nEnter Event/Movie ID to view seat layout: ");
    scanf("%d", &eventId);

    while (ptr != NULL) {
        if (ptr->id == eventId) {
            displaySeats(ptr);
            return;
        }
        ptr = ptr->next;
    }

    printf("\nEvent with ID %d not found.\n", eventId);
}

void linearSearchEventsByCategory(int category) {
    struct Event *ptr = head;
    int count = 0;

    if (ptr == NULL) {
        printf("\nNo events registered in the system.\n");
        return;
    }

    printf("\n========================================================================================================================\n");
    printf("                               LINEAR SEARCH RESULTS FOR CATEGORY: %-20s                \n", getCategoryName(category));
    printf("========================================================================================================================\n");
    printf("%-4s %-36s %-25s %-20s %-10s %-10s %-10s\n", "ID", "Title", "Category", "Artist / Production", "Time", "Regular", "VIP");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    while (ptr != NULL) {
        if (ptr->category == category && ptr->isApproved == 1) {
            printf("%-4d %-36s %-25s %-20s %-10s Rs. %-6.2f Rs. %-6.2f\n",
                   ptr->id, ptr->title, getCategoryName(ptr->category),
                   ptr->artist, ptr->time, ptr->regularPrice, ptr->vipPrice);
            count++;
        }
        ptr = ptr->next;
    }

    if (count == 0) {
        printf("No events found in this category.\n");
    }
    printf("========================================================================================================================\n");
}

static int containsCaseInsensitive(const char *source, const char *pattern) {
    char sLower[100];
    char pLower[100];
    int i;

    for (i = 0; source[i] != '\0' && i < 99; i++) {
        if (source[i] >= 'A' && source[i] <= 'Z') {
            sLower[i] = source[i] - 'A' + 'a';
        } else {
            sLower[i] = source[i];
        }
    }
    sLower[i] = '\0';

    for (i = 0; pattern[i] != '\0' && i < 99; i++) {
        if (pattern[i] >= 'A' && pattern[i] <= 'Z') {
            pLower[i] = pattern[i] - 'A' + 'a';
        } else {
            pLower[i] = pattern[i];
        }
    }
    pLower[i] = '\0';

    return (strstr(sLower, pLower) != NULL);
}

void linearSearchEventsByArtist(const char *artistName) {
    struct Event *ptr = head;
    int count = 0;

    if (ptr == NULL) {
        printf("\nNo events registered in the system.\n");
        return;
    }

    printf("\n========================================================================================================================\n");
    printf("                            LINEAR SEARCH RESULTS FOR ARTIST: %-25s                    \n", artistName);
    printf("========================================================================================================================\n");
    printf("%-4s %-36s %-25s %-20s %-10s %-10s %-10s\n", "ID", "Title", "Category", "Artist / Production", "Time", "Regular", "VIP");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    while (ptr != NULL) {
        if (containsCaseInsensitive(ptr->artist, artistName) && ptr->isApproved == 1) {
            printf("%-4d %-36s %-25s %-20s %-10s Rs. %-6.2f Rs. %-6.2f\n",
                   ptr->id, ptr->title, getCategoryName(ptr->category),
                   ptr->artist, ptr->time, ptr->regularPrice, ptr->vipPrice);
            count++;
        }
        ptr = ptr->next;
    }

    if (count == 0) {
        printf("No events found matching artist '%s'.\n", artistName);
    }
    printf("========================================================================================================================\n");
}

struct Event **createEventPointerArray(int *countOut) {
    int count = 0;
    struct Event *ptr = head;
    struct Event **arr;
    int i;

    while (ptr != NULL) {
        count++;
        ptr = ptr->next;
    }

    *countOut = count;

    if (count == 0) {
        return NULL;
    }

    arr = (struct Event **)malloc(count * sizeof(struct Event *));
    if (arr == NULL) {
        printf("\nMemory allocation failed for event pointer array!\n");
        return NULL;
    }

    ptr = head;
    for (i = 0; i < count; i++) {
        arr[i] = ptr;
        ptr = ptr->next;
    }

    return arr;
}

static int compareCaseInsensitive(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        char c1 = s1[i];
        char c2 = s2[i];
        if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
        if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
        if (c1 != c2) {
            return (c1 - c2);
        }
        i++;
    }
    {
        char c1 = s1[i];
        char c2 = s2[i];
        if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
        if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
        return (c1 - c2);
    }
}

void sortEventsByTitle(struct Event **arr, int count) {
    int i, j;
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (compareCaseInsensitive(arr[j]->title, arr[j + 1]->title) > 0) {
                struct Event *temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

struct Event *binarySearchEventByTitle(const char *targetTitle) {
    int count = 0;
    struct Event **arr;
    int low, high, mid;
    struct Event *result = NULL;

    if (head == NULL) {
        printf("\nNo events registered in the system.\n");
        return NULL;
    }

    arr = createEventPointerArray(&count);
    if (arr == NULL || count == 0) {
        return NULL;
    }

    sortEventsByTitle(arr, count);

    low = 0;
    high = count - 1;

    while (low <= high) {
        int cmp;
        mid = low + (high - low) / 2;
        cmp = compareCaseInsensitive(targetTitle, arr[mid]->title);

        if (cmp == 0) {
            result = arr[mid];
            break;
        } else if (cmp > 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    free(arr);
    return result;
}

int saveEventsToFile(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    struct Event *ptr = head;
    struct EventRecord rec;
    int i, j;

    if (fp == NULL) {
        printf("\nError: Cannot open %s for writing!\n", filename);
        return 0;
    }

    while (ptr != NULL) {
        rec.id = ptr->id;
        strcpy(rec.title, ptr->title);
        rec.category = ptr->category;
        strcpy(rec.artist, ptr->artist);
        strcpy(rec.venue, ptr->venue);
        strcpy(rec.time, ptr->time);
        rec.regularPrice = ptr->regularPrice;
        rec.vipPrice = ptr->vipPrice;
        rec.isApproved = ptr->isApproved;
        for (i = 0; i < 10; i++) {
            for (j = 0; j < 12; j++) {
                rec.seats[i][j] = ptr->seats[i][j];
            }
        }
        if (fwrite(&rec, sizeof(struct EventRecord), 1, fp) != 1) {
            printf("\nError writing to %s!\n", filename);
            fclose(fp);
            return 0;
        }
        ptr = ptr->next;
    }

    fclose(fp);
    return 1;
}

int loadEventsFromFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    struct EventRecord rec;
    struct Event *last = NULL;
    long fileSize;
    long expectedCount;
    long loadedCount = 0;
    int i, j;

    if (fp == NULL) {
        if (errno == ENOENT) {
            return 0;
        }
        printf("\nError: Could not open '%s' due to permission or system error!\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fileSize == 0) {
        printf("\nError: '%s' exists but is empty (0 bytes)! Preserving file without overwrite.\n", filename);
        fclose(fp);
        return -1;
    }

    if (fileSize % (long)sizeof(struct EventRecord) != 0) {
        printf("\nError: '%s' is corrupted (invalid file size: %ld bytes)!\n", filename, fileSize);
        fclose(fp);
        return -1;
    }

    expectedCount = fileSize / (long)sizeof(struct EventRecord);

    while (head != NULL) {
        struct Event *temp = head;
        while (temp->waitlistFront != NULL) {
            struct WaitlistNode *wtemp = temp->waitlistFront;
            temp->waitlistFront = temp->waitlistFront->next;
            free(wtemp);
        }
        head = head->next;
        free(temp);
    }
    head = NULL;

    while (fread(&rec, sizeof(struct EventRecord), 1, fp) == 1) {
        struct Event *newnode = (struct Event *)malloc(sizeof(struct Event));
        if (newnode == NULL) {
            printf("\nMemory allocation failed while loading event!\n");
            fclose(fp);
            while (head != NULL) {
                struct Event *temp = head;
                while (temp->waitlistFront != NULL) {
                    struct WaitlistNode *wtemp = temp->waitlistFront;
                    temp->waitlistFront = temp->waitlistFront->next;
                    free(wtemp);
                }
                head = head->next;
                free(temp);
            }
            return -1;
        }

        newnode->id = rec.id;
        strcpy(newnode->title, rec.title);
        newnode->category = rec.category;
        strcpy(newnode->artist, rec.artist);
        strcpy(newnode->venue, rec.venue);
        strcpy(newnode->time, rec.time);
        newnode->regularPrice = rec.regularPrice;
        newnode->vipPrice = rec.vipPrice;
        newnode->isApproved = rec.isApproved;
        for (i = 0; i < 10; i++) {
            for (j = 0; j < 12; j++) {
                newnode->seats[i][j] = rec.seats[i][j];
            }
        }
        newnode->waitlistFront = NULL;
        newnode->waitlistRear = NULL;
        newnode->next = NULL;

        if (head == NULL) {
            head = newnode;
        } else {
            last->next = newnode;
        }
        last = newnode;

        if (rec.id >= eventIdCounter) {
            eventIdCounter = rec.id + 1;
        }
        loadedCount++;
    }

    if (ferror(fp) || loadedCount != expectedCount) {
        printf("\nError: Incomplete read from '%s' (loaded %ld of %ld records)!\n", filename, loadedCount, expectedCount);
        fclose(fp);
        while (head != NULL) {
            struct Event *temp = head;
            while (temp->waitlistFront != NULL) {
                struct WaitlistNode *wtemp = temp->waitlistFront;
                temp->waitlistFront = temp->waitlistFront->next;
                free(wtemp);
            }
            head = head->next;
            free(temp);
        }
        return -1;
    }

    fclose(fp);
    return 1;
}

static int isDataSafeToSave = 1;

int saveAllData(void) {
    int okEvents;
    int okBookings;
    int okWaitlist;
    int okCancellations;

    if (!isDataSafeToSave) {
        printf("\nError: Data saving disabled to protect existing files from corruption.\n");
        return 0;
    }

    okEvents = saveEventsToFile("events.dat");
    okBookings = saveBookingsToFile("bookings.dat");
    okWaitlist = saveWaitlistToFile("waitlists.dat");
    okCancellations = saveCancellationsToFile("cancellations.dat");

    if (!okEvents || !okBookings || !okWaitlist || !okCancellations) {
        printf("\nError: Failed to save all data files successfully:\n");
        if (!okEvents) {
            printf("  - Could not save events.dat\n");
        }
        if (!okBookings) {
            printf("  - Could not save bookings.dat\n");
        }
        if (!okWaitlist) {
            printf("  - Could not save waitlists.dat\n");
        }
        if (!okCancellations) {
            printf("  - Could not save cancellations.dat\n");
        }
        return 0;
    }
    return 1;
}

static void freeAllLoadedData(void) {
    while (bookingHead != NULL) {
        struct Booking *temp = bookingHead;
        bookingHead = bookingHead->next;
        free(temp);
    }
    while (top != NULL) {
        struct StackNode *temp = top;
        top = top->next;
        free(temp);
    }
    while (head != NULL) {
        struct Event *temp = head;
        while (temp->waitlistFront != NULL) {
            struct WaitlistNode *wtemp = temp->waitlistFront;
            temp->waitlistFront = temp->waitlistFront->next;
            free(wtemp);
        }
        head = head->next;
        free(temp);
    }
}

int loadAllData(void) {
    int eventStatus = loadEventsFromFile("events.dat");
    int bookingStatus;
    int waitlistStatus;
    int cancellationStatus;

    isDataSafeToSave = 1;

    if (eventStatus < 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'events.dat' is unreadable, empty or corrupted!\n");
        printf("Automatic default event creation aborted to prevent overwriting existing data.\n");
        freeAllLoadedData();
        return 0;
    }

    bookingStatus = loadBookingsFromFile("bookings.dat");
    waitlistStatus = loadWaitlistFromFile("waitlists.dat");
    cancellationStatus = loadCancellationsFromFile("cancellations.dat");

    if (eventStatus == 0) {
        if (bookingStatus == 0 && waitlistStatus == 0 && cancellationStatus == 0) {
            printf("\nNo saved data files found. Initializing default sample events...\n");
            initDefaultEvents();
            if (!saveAllData()) {
                printf("\nWarning: Could not save initial default data to disk.\n");
            }
            return 1;
        }

        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'events.dat' is missing while other data files exist!\n");
        printf("Startup aborted to prevent data loss or inconsistency.\n");
        freeAllLoadedData();
        return 0;
    }

    if (bookingStatus < 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'bookings.dat' is unreadable or corrupted!\n");
        freeAllLoadedData();
        return 0;
    }

    if (bookingStatus == 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'bookings.dat' is missing while 'events.dat' exists! Data set is incomplete.\n");
        freeAllLoadedData();
        return 0;
    }

    if (waitlistStatus < 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'waitlists.dat' is unreadable or corrupted!\n");
        freeAllLoadedData();
        return 0;
    }

    if (waitlistStatus == 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'waitlists.dat' is missing while 'events.dat' exists! Data set is incomplete.\n");
        freeAllLoadedData();
        return 0;
    }

    if (cancellationStatus < 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'cancellations.dat' is unreadable or corrupted!\n");
        freeAllLoadedData();
        return 0;
    }

    if (cancellationStatus == 0) {
        isDataSafeToSave = 0;
        printf("\nCritical Warning: 'cancellations.dat' is missing while 'events.dat' exists! Data set is incomplete.\n");
        freeAllLoadedData();
        return 0;
    }

    return 1;
}
