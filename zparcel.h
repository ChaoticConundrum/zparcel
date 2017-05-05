/*******************************************************************************
**                                  LibChaos                                  **
**                                  zparcel.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zuid.h"
#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"
#include "zfile.h"
#include "zmap.h"

namespace LibChaos {

/*! Interface for storing and fetching objects from the LibChaos parcel file format.
 *  Each object is stored, fetched or updated through a unique UUID.
 */
class ZParcel {
public:
    enum parceltype {
        UNKNOWN = 0,
        VERSION1,       //!< Type 1 parcel. No pages, payload in tree node.
        MAX_PARCELTYPE = VERSION1,
    };

    enum parcelstate {
        OPEN,
        CLOSED,
        LOCKED,
    };

    enum parcelopt {
        OPT_NONE        = 0,
        OPT_TAIL_EXTEND = 1,    //! Extend parcel file on tail when full.
    };

    enum {
        NULLOBJ = 0,
        BOOLOBJ,        //!< Boolean object. 1-bit.
        UINTOBJ,        //!< Unsigned integer object. 64-bit.
        SINTOBJ,        //!< Signed integer object. 64-bit.
        FLOATOBJ,       //!< Floating point number object. Double precision.
        ZUIDOBJ,        //!< UUID object.
        BLOBOBJ,        //!< Binary blob object.
        STRINGOBJ,      //!< String object. UTF-8 string.
        LISTOBJ,        //!< List object. Ordered list of UUIDs.
        FILEOBJ,        //!< File object. Includes embedded filename and file content.

        /*! User-defined object types can be created by subclassing ZParcel and defining new types starting with MAX_OBJTYPE.
         *  Example:
         *  \code
         *  enum {
         *      CUSTOM1 = MAX_OBJTYPE,
         *      CUSTOM2,
         *  };
         *  \endcode
         */
        MAX_OBJTYPE,
        UNKNOWNOBJ = 255,
    };

    typedef zu8 objtype;

    enum parcelerror {
        OK = 0,         //!< No error.
        ERR_OPEN,       //!< Error opening file.
        ERR_SEEK,       //!< Error seeking file.
        ERR_READ,       //!< Error reading file.
        ERR_WRITE,      //!< Error writing file.
        ERR_EXISTS,     //!< Object exists.
        ERR_NOEXIST,    //!< Object does not exist.
        ERR_CRC,        //!< CRC mismatch.
        ERR_TRUNC,      //!< Payload is truncated by end of file.
        ERR_TREE,       //!< Bad tree structure.
        ERR_FREELIST,   //!< Bad freelist structure.
        ERR_NOFREE,     //!< No free nodes.
        ERR_SIG,        //!< Bad file signature.
        ERR_VERSION,    //!< Bad file header version.
        ERR_MAX_DEPTH,  //!< Exceeded maximum tree depth.
        ERR_MAGIC,      //!< Bad object magic number.
    };

protected:
    struct ObjectInfo;

public:
    ZParcel();
    ZParcel(const ZParcel &other) = delete;
    ~ZParcel();

    /*! Create new parcel file and open it.
     *  This will overwrite an existing file.
     *  \exception ZException Failed to create file.
     */
    parcelerror create(ZBlockAccessor *file, parcelopt opt);

    /*! Open existing parcel.
     *  \exception ZException Failed to open file.
     */
    parcelerror open(ZBlockAccessor *file);

    //! Close file handles.
    void close();

    //! Check if \a id exists in the parcel.
    bool exists(ZUID id);
    //! Get type of parcel object.
    objtype getType(ZUID id);

    /*! Store null in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeNull(ZUID id);
    /*! Store bool in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeBool(ZUID id, bool bl);
    /*! Store unsigned int in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeUint(ZUID id, zu64 num);
    /*! Store signed int in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeSint(ZUID id, zs64 num);
    /*! Store float in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeFloat(ZUID id, double num);
    /*! Store ZUID in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeZUID(ZUID id, ZUID uid);
    /*! Store blob in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeBlob(ZUID id, ZBinary blob);
    /*! Store string in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeString(ZUID id, ZString str);
    /*! Store list in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeList(ZUID id, ZList<ZUID> list);
    /*! Store file reference in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeFile(ZUID id, ZPath path);

    /*! Fetch bool from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    bool fetchBool(ZUID id);
    /*! Fetch unsigned int from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    zu64 fetchUint(ZUID id);
    /*! Fetch signed int from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    zs64 fetchSint(ZUID id);
    /*! Fetch float from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    double fetchFloat(ZUID id);
    /*! Fetch ZUID from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZUID fetchZUID(ZUID id);
    /*! Fetch blob from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZBinary fetchBlob(ZUID id);
    /*! Fetch reader for blob from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZPointer<ZBlockAccessor> fetchBlobReader(ZUID id);
    /*! Fetch string from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZString fetchString(ZUID id);
    /*! Fetch list from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZList<ZUID> fetchList(ZUID id);
    /*! Fetch file object from parcel.
     *  If \a offset is not null, the offset of the file payload is written at \a offset.
     *  If \a size is not null, the size of the file payload is written at \a size.
     *  \return An accessor for the file contents.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    parcelerror fetchFile(ZUID id, ZUID &nameid, ZUID &dataid);

    /*! Remove an object from the parcel.
     *  \exception ZException Parcel not open.
      */
    parcelerror removeObject(ZUID id);

    ZUID getRoot();
    parcelerror setRoot(ZUID id);

    void listObjects();

    void _listStep(zu64 next, zu16 depth);

    //! Get string name of object type.
    static ZString typeName(objtype type);
    //! Get string for error.
    static ZString errorStr(parcelerror err);

protected:
    //! Compute the size of an object node payload.
    zu64 _objectSize(objtype type, zu64 size);
    /*! Store a new object with \a id and \a type.
     *  The contents of \a data are written into the payload of the new object.
     *  If \a trailsize > 0, indicates the number of bytes that should be reserved in the payload,
     *  beyond the size of \a data.
     */
    parcelerror _storeObject(ZUID id, objtype type, const ZBinary &data, zu64 reserve = 0);
    //! Get object info struct.
    parcelerror _getObjectInfo(ZUID id, ObjectInfo *info);

private:
    /*! Allocate node of at least \a size.
     *  Offset and actual size of the node are returned in \a offset and \a nsize.
     */
    parcelerror _nodeAlloc(zu64 size, zu64 *offset, zu64 *nsize);
    //! Add node at \a offset with \a size to the freelist.
    parcelerror _nodeFree(zu64 offset, zu64 size);

protected:
    class ParcelObjectAccessor : public ZBlockAccessor {
    public:
        ParcelObjectAccessor(ZBlockAccessor *file, zu64 offset, zu64 size) :
            _file(file), _base(offset), _pos(0), _size(size){

        }

        // ZReader interface
        zu64 available() const {
            return (_base + _size) - _pos;
        }
        zu64 read(zbyte *dest, zu64 size);

        // ZWriter interface
        zu64 write(const zbyte *src, zu64 size);

        // ZPosition interface
        zu64 tell() const {
            return _pos;
        }
        zu64 seek(zu64 pos){
            _pos = MIN(pos, _base + _size);
            return _pos;
        }
        bool atEnd() const {
            return (tell() == _base + _size);
        }

    private:
        ZBlockAccessor *const _file;
        const zu64 _base;
        zu64 _pos;
        const zu64 _size;
    };

    struct ObjectInfo {
        zu64 tree;      // Tree node offset
        zu64 parent;    // Parent tree node offset
        zu64 lnode;     // Left child tree node offset
        zu64 rnode;     // Right child tree node offset

        objtype type;   // Payload type
        union {
            zbyte payload[16];
            struct {
                zu64 offset;    // Payload offset
                zu64 size;      // Payload size
            } data;
        };

        ZPointer<ParcelObjectAccessor> accessor;
    };

private:
    class ParcelHeader {
    public:
        ParcelHeader(ZParcel *parcel, zu64 addr) : file(parcel->_file), offset(addr){}

        parcelerror read();
        parcelerror write();

        static const zu64 NODE_SIZE = (7 + 1 + 4 + 8 + 8 + 8 + 8 + ZUID_SIZE + 4);

    public:
        // 7 byte signature
        zu8 version;
        zu32 flags;
        zu64 treehead;
        zu64 freehead;
        zu64 freetail;
        zu64 tailptr;
        ZUID root;
        // 4 byte crc

    private:
        ZBlockAccessor *const file;
        const zu64 offset;
    };

private:
    class ParcelTreeNode {
    public:
        ParcelTreeNode(ZParcel *parcel, zu64 addr) : file(parcel->_file), offset(addr){}

        parcelerror read();
        parcelerror write();

        static const zu64 NODE_SIZE = (4 + ZUID_SIZE + 8 + 8 + 1 + 1 + 4 + 16);

    public:
        // 4 byte magic
        ZUID uid;
        zu64 lnode;
        zu64 rnode;
        zu8 type;
        zu8 extra;
        // 4 byte crc
        zbyte payload[16];

        struct {
            zu64 size;
            zu64 offset;
        } data;


    private:
        ZBlockAccessor *const file;
        const zu64 offset;
    };

private:
    class ParcelFreeNode {
    public:
        ParcelFreeNode(ZParcel *parcel, zu64 addr) : file(parcel->_file), offset(addr){}

        parcelerror read();
        parcelerror write();

        static const zu64 NODE_SIZE = (4 + 8 + 8 + 4);

    public:
        zu64 next;
        zu64 size;

    private:
        ZBlockAccessor *const file;
        const zu64 offset;
    };

private:
    parcelstate _state;
    ZBlockAccessor *_file;
    ParcelHeader *_header;
    ZMap<ZUID, ObjectInfo> _cache;
};

} // namespace LibChaos

#endif // ZPARCEL_H
