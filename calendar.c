#include <stdio.h>      // printf, scanf, fgets
#include <stdlib.h>     // malloc, free, rand, srand
#include <string.h>     // strcpy, strlen, strcmp
#include <time.h>       // time (for srand)
#include <ctype.h>      // isspace

/* =========================
   Data model
   ========================= */

/* One event inside a day: e.g., 1430 = 14:30, title = "Doctor" */
typedef struct Evt {
    int timeHHMM;            // e.g., 930 for 09:30, 1430 for 14:30
    char title[80];          // short event title
    struct Evt *next;        // linked list next
} Evt;

/* One treap node = one calendar day */
typedef struct Node {
    int date;                // key: YYYYMMDD as an int (e.g., 20251107)
    int prior;               // heap priority (random)
    struct Node *l, *r;      // left/right children (BST by date)
    Evt *head;               // linked list of events for this date
} Node;

/* Make a new event node */
Evt* make_event(int hhmm, const char *title) {
    Evt *e = (Evt*)malloc(sizeof(Evt));
    e->timeHHMM = hhmm;
    strncpy(e->title, title, sizeof(e->title)-1);
    e->title[sizeof(e->title)-1] = '\0';
    e->next = NULL;
    return e;
}

/* Make a new treap node for a date (starts with one event) */
Node* make_node(int date, int hhmm, const char *title) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->date = date;
    n->prior = rand(); // random priority for heap balance
    n->l = n->r = NULL;
    n->head = make_event(hhmm, title);
    return n;
}

/* =========================
   Helper functions for date/time
   ========================= */

/* Pack Y, M, D into YYYYMMDD int */
int pack_date(int y, int m, int d) {
    return y*10000 + m*100 + d;
}

/* Unpack YYYYMMDD into Y, M, D */
void unpack_date(int date, int *y, int *m, int *d) {
    *y = date / 10000;
    *m = (date / 100) % 100;
    *d = date % 100;
}

/* Print date as YYYY-MM-DD */
void print_date(int date) {
    int y,m,d; unpack_date(date, &y, &m, &d);
    printf("%04d-%02d-%02d", y, m, d);
}

/* Print time as HH:MM from 4-digit int (e.g., 930 → 09:30) */
void print_time(int hhmm) {
    int hh = hhmm / 100, mm = hhmm % 100;
    printf("%02d:%02d", hh, mm);
}

/* =========================
   Event list ops (inside a day)
   ========================= */

/* Insert event into the day's linked list sorted by time.
   Conflict rule: if same HHMM already exists, report conflict and do not add. */
int add_event_to_day(Node *day, int hhmm, const char *title) {
    // Check conflict
    for (Evt *p = day->head; p != NULL; p = p->next) {
        if (p->timeHHMM == hhmm) {
            printf("⚠️  Conflict: event already at ");
            print_time(hhmm);
            printf(" on "); print_date(day->date); printf(".\n");
            return 0; // not added
        }
    }
    // Insert keeping times sorted
    Evt *e = make_event(hhmm, title);
    if (!day->head || hhmm < day->head->timeHHMM) {
        e->next = day->head;
        day->head = e;
        return 1;
    }
    Evt *cur = day->head;
    while (cur->next && cur->next->timeHHMM < hhmm) cur = cur->next;
    e->next = cur->next;
    cur->next = e;
    return 1;
}

/* Print all events in a day */
void print_day(Node *day) {
    if (!day) return;
    print_date(day->date);
    printf(":\n");
    for (Evt *p = day->head; p != NULL; p = p->next) {
        printf("  - ");
        print_time(p->timeHHMM);
        printf("  %s\n", p->title);
    }
}

/* =========================
   Treap core: split & merge
   ========================= */

/* split(t, key, *L, *R):
   L gets keys < key; R gets keys >= key */
void split(Node *t, int key, Node **L, Node **R) {
    if (!t) { *L = *R = NULL; return; }
    if (t->date < key) {
        // t stays in L; split its right child
        split(t->r, key, &t->r, R);
        *L = t;
    } else {
        // t goes to R; split its left child
        split(t->l, key, L, &t->l);
        *R = t;
    }
}

/* merge(L, R): all keys in L < keys in R */
Node* merge(Node *L, Node *R) {
    if (!L) return R;
    if (!R) return L;
    if (L->prior > R->prior) {
        L->r = merge(L->r, R);
        return L;
    } else {
        R->l = merge(L, R->l);
        return R;
    }
}

/* =========================
   Basic treap operations
   ========================= */

/* Find a day by date (standard BST search) */
Node* find(Node *t, int date) {
    while (t) {
        if (date == t->date) return t;
        else if (date < t->date) t = t->l;
        else t = t->r;
    }
    return NULL;
}

/* Insert (date, hhmm, title)
   - If date exists: append into its event list (with conflict detection).
   - Else: create a new node using split/merge and return new root. */
Node* insert_event(Node *t, int date, int hhmm, const char *title) {
    Node *ex = find(t, date);
    if (ex) {
        add_event_to_day(ex, hhmm, title);
        return t; // root unchanged
    }
    // Date not present: create a new node and treap-insert it
    Node *left = NULL, *right = NULL;
    split(t, date, &left, &right);
    Node *newNode = make_node(date, hhmm, title);
    return merge(merge(left, newNode), right);
}

/* In-order traversal (sorted by date), printing days and events */
void inorder_print(Node *t) {
    if (!t) return;
    inorder_print(t->l);
    print_day(t);
    inorder_print(t->r);
}

/* =========================
   Range print for a month
   ========================= */

/* Print all events in a given month (YYYY, MM) using split range.
   We split T into <lo, [lo..hi], >hi, print the middle, then merge back. */
/* Helper: count & print in-order */
int inorder_print_and_count(Node *t) {
    if (!t) return 0;
    int cnt = inorder_print_and_count(t->l);
    print_day(t);
    cnt++;
    cnt += inorder_print_and_count(t->r);
    return cnt;
}


/* Updated month_range_print with “No events” message */
Node* month_range_print(Node *t, int year, int month) {
    int lo = pack_date(year, month, 1);
    int hi = pack_date(year, month, 31);

    Node *A=NULL, *rest=NULL, *B=NULL, *C=NULL;
    split(t, lo, &A, &rest);
    split(rest, hi + 1, &B, &C);

    printf("\n📅 Events in %04d-%02d:\n", year, month);
    int count = inorder_print_and_count(B);

    if (count == 0) {
        printf("📭 No events found in %04d-%02d.\n", year, month);
    }

    // Merge treap back
    rest = merge(B, C);
    t = merge(A, rest);
    return t;
}


/* =========================
   Next upcoming event (lower_bound by date)
   ========================= */

/* Find node with the smallest date >= fromDate */
Node* lower_bound(Node *t, int fromDate) {
    Node *ans = NULL;
    while (t) {
        if (t->date >= fromDate) {
            ans = t;        // candidate
            t = t->l;       // try to find smaller >= fromDate
        } else {
            t = t->r;
        }
    }
    return ans;
}

/* Print the first event of the next upcoming day (smallest time of that day) */
void print_next_upcoming(Node *t, int fromDate) {
    Node *n = lower_bound(t, fromDate);
    if (!n) {
        printf("\n⏭️  No upcoming events on or after ");
        print_date(fromDate);
        printf(".\n");
        return;
    }
    // first event of the day (list is sorted by time)
    printf("\n⏭️  Next upcoming day: ");
    print_date(n->date);
    printf("\n");
    if (n->head) {
        printf("   ");
        print_time(n->head->timeHHMM);
        printf("  %s\n", n->head->title);
    } else {
        printf("   (No events stored, day exists)\n");
    }
}

/* =========================
   Memory cleanup
   ========================= */

void free_events(Evt *e) {
    while (e) { Evt *nx = e->next; free(e); e = nx; }
}

void free_treap(Node *t) {
    if (!t) return;
    free_treap(t->l);
    free_treap(t->r);
    free_events(t->head);
    free(t);
}

/* =========================
   Tiny interactive demo (menu)
   ========================= */

/* Read a whole line safely (used to get event titles including spaces) */
void read_line(char *buf, size_t n) {
    if (fgets(buf, (int)n, stdin)) {
        // strip trailing newline
        size_t L = strlen(buf);
        if (L && buf[L-1] == '\n') buf[L-1] = '\0';
    } else {
        buf[0] = '\0';
    }
}

/* Skip leftover newline after scanf */
void eat_newline(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
/* Check if year, month, day are valid */
int is_valid_date(int y, int m, int d) {
    if (y < 1900 || m < 1 || m > 12) return 0;

    int days_in_month[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int leap = (y%4==0 && (y%100!=0 || y%400==0));
    if (leap) days_in_month[2] = 29;

    return (d >= 1 && d <= days_in_month[m]);
}

/* Check if time is valid 24-hour format */
int is_valid_time(int hh, int mm) {
    return (hh >= 0 && hh < 24 && mm >= 0 && mm < 60);
}

int main(void) {
    srand((unsigned)time(NULL));

    Node *root = NULL;

    while (1) {
        printf("\n*************************************************************\n");
        printf("\n==== Priority Calendar (Treap) ====\n");
        printf("1) Add event\n");
        printf("2) Show all (by date)\n");
        printf("3) Show month (YYYY MM)\n");
        printf("4) Next upcoming from (YYYY MM DD)\n");
        printf("0) Exit\n");
        printf("\n*************************************************************\n");
        printf("Choose: ");
        int choice;
        if (scanf("%d", &choice) != 1) break;
        eat_newline();
        
        if (choice == 0) break;

        if (choice == 1) {
    int y, m, d, hh, mm;
    char title[80];

    printf("Enter date (YYYY MM DD): ");
    if (scanf("%d %d %d", &y, &m, &d) != 3) {
        eat_newline();
        printf("❌ Oops! Invalid input! Please enter numbers for date.\n");
        continue;
    }

    // Check date format
    if (!is_valid_date(y, m, d)) {
        eat_newline();
        printf("❌ Oops! Invalid date! Please enter a valid one (YYYY MM DD).\n");
        continue;
    }

    printf("Enter time (HH MM 24-hr): ");
    if (scanf("%d %d", &hh, &mm) != 2) {
        eat_newline();
        printf("❌ Oops! Invalid input! Please enter numbers for time.\n");
        continue;
    }

    // Check time format
    if (!is_valid_time(hh, mm)) {
        eat_newline();
        printf("❌ Oops! Invalid time! Please enter a valid 24-hour time (HH MM).\n");
        continue;
    }

    eat_newline();
    printf("Enter title: ");
    read_line(title, sizeof(title));

    int date = pack_date(y, m, d);
    int hhmm = hh * 100 + mm;
    root = insert_event(root, date, hhmm, title);
    //printf("✅ Event added\n");
} else if (choice == 2) {
            printf("\n🗂️  All events:\n");
            inorder_print(root);

        } else if (choice == 3) {
            int y, m;
            printf("Enter month (YYYY MM): ");
            if (scanf("%d %d", &y, &m) != 2) { eat_newline(); continue; }
            eat_newline();
            root = month_range_print(root, y, m);

        } else if (choice == 4) {
            int y, m, d;
            printf("Enter from date (YYYY MM DD): ");
            if (scanf("%d %d %d", &y, &m, &d) != 3) { eat_newline(); continue; }
            eat_newline();
            int fromDate = pack_date(y, m, d);
            print_next_upcoming(root, fromDate);

        } else {
            printf("Unknown choice.\n");
        }
    }

    free_treap(root);
    return 0;
}
