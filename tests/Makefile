CC=gcc
LDLIBS=-lcmocka -lm
SRC=/home/sources/
INCLUDES=-I/home/includes
TESTSDIR=tests
CFLAGS=-fopenmp -std=gnu99

metrics:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/metrics.c $(TESTSDIR)/metrics_test.c \
		-o metrics_test $(LDLIBS)

classes:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/classes.c $(TESTSDIR)/test_classes.c \
		-o test_classes $(LDLIBS)

dissimilarities:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/attributes.c $(SRC)/dissimilarities.c \
		$(SRC)/values.c $(SRC)/classes.c $(TESTSDIR)/test_dissimilarities.c \
		-o test_dissimilarities $(LDLIBS)

values:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/attributes.c $(SRC)/values.c \
		$(SRC)/classes.c $(TESTSDIR)/test_values.c -o test_values $(LDLIBS)

attributes:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/attributes.c $(SRC)/classes.c \
		$(SRC)/values.c $(TESTSDIR)/test_attributes.c -o test_attributes $(LDLIBS)

rules:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/rules.c $(SRC)/classes.c \
		$(SRC)/dissimilarities.c $(SRC)/values.c $(TESTSDIR)/test_rules.c \
		-o test_rules $(LDLIBS)

riona:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/riona.c $(TESTSDIR)/test_riona.c \
		$(SRC)/classes.c $(SRC)/dissimilarities.c $(SRC)/timing.c \
		$(SRC)/rules.c $(SRC)/values.c $(SRC)/attributes.c -o test_riona $(LDLIBS)

all: metrics classes dissimilarities values attributes rules riona

clean:
	rm -f metrics_test test_classes test_dissimilarities test_values \
		test_attributes test_rules riona