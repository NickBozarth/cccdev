#ifndef ALG_SETTINGS
#define ALG_SETTINGS



struct caesar_test_settings {
    int rot;
};




union algorithm_settings {
    struct caesar_test_settings caesar;
};

#endif /* ALG_SETTINGS */
