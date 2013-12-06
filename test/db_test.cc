#include "db/options.h"
#include "db/comparator.h"
#include "db/db_impl.h"
#include "fs/env.h"
#include "util/logger.h"
#include "util/timestamp.h"

using namespace yodb;

const uint64_t kMaxCount = 1000 * 1000;

void write(DBImpl* db)
{
    for (uint64_t i = 0; i < kMaxCount; i++) {
        char buffer[16] = {0};
        sprintf(buffer, "%08ld", i);

        Slice value(buffer, strlen(buffer));
        Slice key = value;

        db->put(key, value);
    } 
}

void read(DBImpl* db)
{
    for (uint64_t i = 0; i < kMaxCount; i++) {
        char buffer[16] = {0};
        sprintf(buffer, "%08ld", i);

        Slice key(buffer, strlen(buffer));
        Slice value;

        db->get(key, value);

        if (((i+1) % 10000) == 0)
            LOG_INFO << "Read 10000 success";

        if (key.compare(value) != 0)
            LOG_INFO << "key=" << key.data()
                     << ", value=" << value.data() << ", failed";
        // else 
        //    printf("key=%s, get=%s, success\n", key.data(), value.data());

    } 
}

int main()
{
    Options opts;
    opts.comparator = new LexicalComparator();
    opts.env = new Env("/home/kedebug/develop/yodb/bin");

    DBImpl* db = new DBImpl("second", opts);
    db->init();

    Timestamp begin = Timestamp::now();

    write(db);

    Timestamp end = Timestamp::now();
    LOG_INFO << "write: " << kMaxCount 
             << Fmt(", key, costs time: %f", time_interval(end, begin));

    delete db;

    db = new DBImpl("second", opts); 
    db->init();
    read(db);

    end = Timestamp::now();
    LOG_INFO << "read: " << kMaxCount 
             << Fmt(", key, costs time: %f", time_interval(end, begin));

    delete db;
    delete opts.env;
    delete opts.comparator;
}
