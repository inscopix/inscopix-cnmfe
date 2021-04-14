#include "isxUtilities.h"

namespace isx
{
    size_t getDataTypeSizeInBytes(DataType inDataType)
    {
        switch (inDataType)
        {
            case DataType::F32:
            {
                return sizeof(float);
            }
            case DataType::U16:
            {
                return sizeof(uint16_t);
            }
            case DataType::U8:
            {
                return sizeof(uint8_t);
            }
            default:
            {
                return 0;
            }
        }
    }
}
