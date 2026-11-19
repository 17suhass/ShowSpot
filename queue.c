#include "project.h"

int waitTokenCounter = 1;

int addToWaitingList(struct Event *eventPtr, char buyerName[]) {
  struct WaitlistNode *newnode;

  if (eventPtr == NULL) {
    return 0;
  }

  newnode = (struct WaitlistNode *)malloc(sizeof(struct WaitlistNode));
  if (newnode == NULL) {
    printf("\nMemory allocation failed for waiting list!\n");
    return 0;
  }

  newnode->token = waitTokenCounter++;
  newnode->eventId = eventPtr->id;
  strcpy(newnode->buyerName, buyerName);
  newnode->next = NULL;

  if (eventPtr->waitlistRear == NULL) {
    eventPtr->waitlistFront = eventPtr->waitlistRear = newnode;
  } else {
    eventPtr->waitlistRear->next = newnode;
    eventPtr->waitlistRear = newnode;
  }

  printf("\n======================================================\n");
  printf("            ADDED TO WAITING LIST QUEUE               \n");
  printf("======================================================\n");
  printf("Waitlist Token : #%d\n", newnode->token);
  printf("Buyer Name     : %s\n", newnode->buyerName);
  printf("Event Title    : %s\n", eventPtr->title);
  printf("Queue Position : Added in FIFO order.\n");
  printf("======================================================\n");

  return 1;
}

void displayWaitingList(struct Event *eventPtr) {
  struct WaitlistNode *ptr;

  if (eventPtr == NULL) {
    return;
  }

  ptr = eventPtr->waitlistFront;
  if (ptr == NULL) {
    printf("\nWaiting list for '%s' is currently empty.\n", eventPtr->title);
    return;
  }

  printf("\n==================================================================="
         "==============\n");
  printf("                 WAITING LIST QUEUE FOR: %s\n", eventPtr->title);
  printf("====================================================================="
         "============\n");
  printf("%-10s %-12s %-25s\n", "Token", "Event ID", "Buyer Name");
  printf("---------------------------------------------------------------------"
         "------------\n");

  while (ptr != NULL) {
    printf("#%-9d %-12d %-25s\n", ptr->token, ptr->eventId, ptr->buyerName);
    ptr = ptr->next;
  }
  printf("====================================================================="
         "============\n");
}

void joinWaitingListPortal(void) {
  int eventId;
  char buyerName[50];
  struct Event *ptr = head;

  displayEvents(0);
  if (head == NULL) {
    return;
  }

  printf("\nEnter Event ID to join its waiting list: ");
  scanf("%d", &eventId);

  while (ptr != NULL && ptr->id != eventId) {
    ptr = ptr->next;
  }

  if (ptr == NULL || ptr->isApproved == 0) {
    printf("\nInvalid Event ID or event is not approved yet!\n");
    return;
  }

  if (hasAvailableSeats(ptr)) {
    printf("\nThis event still has available seats! Please use option 5 to "
           "book a seat directly.\n");
    return;
  }

  printf("Enter Buyer/Attendee Name: ");
  scanf(" %49[^\n]", buyerName);

  if (addToWaitingList(ptr, buyerName)) {
    saveAllData();
  }
}

void viewWaitingListPortal(void) {
  int eventId;
  struct Event *ptr = head;

  displayEvents(0);
  if (head == NULL) {
    return;
  }

  printf("\nEnter Event ID to view its waiting list: ");
  scanf("%d", &eventId);

  while (ptr != NULL && ptr->id != eventId) {
    ptr = ptr->next;
  }

  if (ptr == NULL) {
    printf("\nEvent with ID %d not found.\n", eventId);
    return;
  }

  displayWaitingList(ptr);
}

int saveWaitlistToFile(const char *filename) {
  FILE *fp = fopen(filename, "wb");
  struct Event *eventPtr = head;
  struct WaitlistNode *ptr;
  struct WaitlistRecord rec;

  if (fp == NULL) {
    printf("\nError: Cannot open %s for writing!\n", filename);
    return 0;
  }

  while (eventPtr != NULL) {
    ptr = eventPtr->waitlistFront;
    while (ptr != NULL) {
      rec.token = ptr->token;
      rec.eventId = ptr->eventId;
      strcpy(rec.buyerName, ptr->buyerName);
      if (fwrite(&rec, sizeof(struct WaitlistRecord), 1, fp) != 1) {
        printf("\nError writing to %s!\n", filename);
        fclose(fp);
        return 0;
      }
      ptr = ptr->next;
    }
    eventPtr = eventPtr->next;
  }

  fclose(fp);
  return 1;
}

int loadWaitlistFromFile(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  struct WaitlistRecord rec;
  struct Event *eventPtr;
  struct WaitlistNode *newnode;
  long fileSize;
  long expectedCount;
  long loadedCount = 0;

  if (fp == NULL) {
    if (errno == ENOENT) {
      return 0;
    }
    printf("\nError: Could not open '%s' due to permission or system error!\n",
           filename);
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  eventPtr = head;
  while (eventPtr != NULL) {
    while (eventPtr->waitlistFront != NULL) {
      struct WaitlistNode *temp = eventPtr->waitlistFront;
      eventPtr->waitlistFront = eventPtr->waitlistFront->next;
      free(temp);
    }
    eventPtr->waitlistRear = NULL;
    eventPtr = eventPtr->next;
  }

  if (fileSize == 0) {
    fclose(fp);
    return 1;
  }

  if (fileSize % (long)sizeof(struct WaitlistRecord) != 0) {
    printf("\nError: '%s' is corrupted (invalid file size: %ld bytes)!\n",
           filename, fileSize);
    fclose(fp);
    return -1;
  }

  expectedCount = fileSize / (long)sizeof(struct WaitlistRecord);

  while (fread(&rec, sizeof(struct WaitlistRecord), 1, fp) == 1) {
    eventPtr = head;
    while (eventPtr != NULL && eventPtr->id != rec.eventId) {
      eventPtr = eventPtr->next;
    }

    if (eventPtr == NULL) {
      loadedCount++;
      continue;
    }

    newnode = (struct WaitlistNode *)malloc(sizeof(struct WaitlistNode));
    if (newnode == NULL) {
      printf("\nMemory allocation failed while loading waitlist!\n");
      fclose(fp);
      eventPtr = head;
      while (eventPtr != NULL) {
        while (eventPtr->waitlistFront != NULL) {
          struct WaitlistNode *temp = eventPtr->waitlistFront;
          eventPtr->waitlistFront = eventPtr->waitlistFront->next;
          free(temp);
        }
        eventPtr->waitlistRear = NULL;
        eventPtr = eventPtr->next;
      }
      return -1;
    }

    newnode->token = rec.token;
    newnode->eventId = rec.eventId;
    strcpy(newnode->buyerName, rec.buyerName);
    newnode->next = NULL;

    if (eventPtr->waitlistRear == NULL) {
      eventPtr->waitlistFront = eventPtr->waitlistRear = newnode;
    } else {
      eventPtr->waitlistRear->next = newnode;
      eventPtr->waitlistRear = newnode;
    }

    if (rec.token >= waitTokenCounter) {
      waitTokenCounter = rec.token + 1;
    }
    loadedCount++;
  }

  if (ferror(fp) || loadedCount != expectedCount) {
    printf("\nError: Incomplete read from '%s'!\n", filename);
    fclose(fp);
    eventPtr = head;
    while (eventPtr != NULL) {
      while (eventPtr->waitlistFront != NULL) {
        struct WaitlistNode *temp = eventPtr->waitlistFront;
        eventPtr->waitlistFront = eventPtr->waitlistFront->next;
        free(temp);
      }
      eventPtr->waitlistRear = NULL;
      eventPtr = eventPtr->next;
    }
    return -1;
  }

  fclose(fp);
  return 1;
}
