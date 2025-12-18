#ifndef DATA_COLLECTION_FACTORY_H
#define DATA_COLLECTION_FACTORY_H

#include "DataCollectionDS80.h"
#include "DataCollectionA1088.h"

typedef enum class _DataCollectMode
{
    DS80,
    A1088,
}DataCollectMode;

//模组DS80
class DataCollectionFactory
{
public:
    ~DataCollectionFactory() {};

    static std::shared_ptr<i_DataCollectionModule> createDataCollectionMoudle(DataCollectMode mode)
    {
        std::shared_ptr<i_DataCollectionModule> dataCollection = nullptr;
        switch (mode)
        {
        case DataCollectMode::DS80:
            dataCollection = std::make_shared<DataCollectionDS80>();
            break;
        case DataCollectMode::A1088:
            dataCollection = std::make_shared<DataCollectionA1088>();
            break;
        }
        return dataCollection;
    }

};

#endif // DATA_COLLECTION_DS80_H
