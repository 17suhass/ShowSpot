#ifndef PROJECT_H
#define PROJECT_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct WaitlistNode {
  int token;
  int eventId;
  char buyerName[50];
  struct WaitlistNode *next;
};

struct Event {
  int id;
  char title[60];
  int category;
  char artist[60];
  char venue[60];
  char time[20];
  float regularPrice;
  float vipPrice;
  char seats[10][12];
  int isApproved;
  struct WaitlistNode *waitlistFront;
  struct WaitlistNode *waitlistRear;
  struct Event *next;
};

struct Booking {
  int bookingId;
  char buyerName[50];
  int eventId;
  char eventTitle[60];
  int category;
  char row;
  int col;
  float price;
  struct Booking *next;
};

struct StackNode {
  int bookingId;
  char buyerName[50];
  int eventId;
  char eventTitle[60];
  int category;
  char row;
  int col;
  float price;
  int autoAllocated;
  int autoBookingId;
  char autoBuyerName[50];
  int autoToken;
  struct StackNode *next;
};

struct EventRecord {
  int id;
  char title[60];
  int category;
  char artist[60];
  char venue[60];
  char time[20];
  float regularPrice;
  float vipPrice;
  int isApproved;
  char seats[10][12];
};

struct BookingRecord {
  int bookingId;
  char buyerName[50];
  int eventId;
  char eventTitle[60];
  int category;
  char row;
  int col;
  float price;
};

struct WaitlistRecord {
  int token;
  int eventId;
  char buyerName[50];
};

struct StackRecord {
  int bookingId;
  char buyerName[50];
  int eventId;
  char eventTitle[60];
  int category;
  char row;
  int col;
  float price;
  int autoAllocated;
  int autoBookingId;
  char autoBuyerName[50];
  int autoToken;
};

extern struct Event *head;
extern struct Booking *bookingHead;
extern struct StackNode *top;

extern int bookingCounter;
extern int eventIdCounter;
extern int waitTokenCounter;

const char *getCategoryName(int category);
void addEvent(int id, char title[], int category, char artist[], char venue[],
              char time[], float regularPrice, float vipPrice, int isApproved);
void initDefaultEvents(void);
void displayEvents(int filterMode);
void displaySeats(struct Event *ptr);
void viewSeatLayout(void);
int hasAvailableSeats(struct Event *ptr);

void linearSearchEventsByCategory(int category);
void linearSearchEventsByArtist(const char *artistName);
struct Event **createEventPointerArray(int *countOut);
void sortEventsByTitle(struct Event **arr, int count);
struct Event *binarySearchEventByTitle(const char *targetTitle);

void bookTicket(void);
void cancelTicket(void);
void displayBookings(void);
void linearSearchBookingById(int bookingId);

int pushCancellation(struct Booking *ptr, int autoAllocated, int autoBookingId,
                     const char autoBuyerName[], int autoToken);
void undoCancellation(void);

int addToWaitingList(struct Event *eventPtr, char buyerName[]);
void displayWaitingList(struct Event *eventPtr);
void joinWaitingListPortal(void);
void viewWaitingListPortal(void);

void searchMenu(void);
void artistRegisterEvent(void);
void nodalOfficerOverview(void);
void nodalOfficerAddMovie(void);
void buyerPortal(void);
void artistPortal(void);
void nodalOfficerPortal(void);

int saveEventsToFile(const char *filename);
int loadEventsFromFile(const char *filename);
int saveBookingsToFile(const char *filename);
int loadBookingsFromFile(const char *filename);
int saveWaitlistToFile(const char *filename);
int loadWaitlistFromFile(const char *filename);
int saveCancellationsToFile(const char *filename);
int loadCancellationsFromFile(const char *filename);
int saveAllData(void);
int loadAllData(void);

#endif
