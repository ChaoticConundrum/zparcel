#ifndef ZPARCEL4PARSER
#define ZPARCEL4PARSER

#include "zfile.h"
#include "zmap.h"
#include "zlist.h"
#include "zuid.h"

#define ZPARCEL_4_NULL          0
#define ZPARCEL_4_UNSIGNEDINT   1
#define ZPARCEL_4_SIGNEDINT     2
#define ZPARCEL_4_ZUID          3
#define ZPARCEL_4_STRING        4
#define ZPARCEL_4_FILE          5
#define ZPARCEL_4_BINARY        6
#define ZPARCEL_4_FLOAT         7

namespace LibChaos {

class ZParcel4Parser {
public:
    typedef zu32 pageid;
    enum pagetype {
        freepage,
        fieldpage,
        freelistpage,
        indexpage,
        recordpage,
        blobpage,
        historypage,
        headpage,
    };

    typedef zu16 fieldid;
    typedef zu8 fieldtype;

    struct Field {
        fieldid id;
        ZBinary data;
    };
    typedef ZList<Field> FieldList;

public:
    ZParcel4Parser(ZFile *file);

    bool create();
    bool open();

    zu32 getPageSize() const { return _pagesize; }
    zu32 getMaxPages() const { return _maxpages; }

    void setPageSize(zu8 power);
    void setMaxPages(zu32 pages);

    fieldid addField(ZString name, fieldtype type);
    fieldid getFieldId(ZString name);
    fieldtype getFieldType(fieldid id);

    bool addRecord(FieldList fields);

    void addUintRecord(fieldid field, zu64 num);
    void addSintRecord(fieldid field, zs64 num);
    void addZUIDRecord(fieldid field, ZUID uid);
    void addFloatRecord(fieldid field, double flt);
    void addStringRecord(fieldid field, ZString str);
    void addBinaryRecord(fieldid field, ZBinary bin);
    void addFileRecord(fieldid field, ZPath file);

    static fieldtype fieldTypeNameToFieldType(ZString name);
    static ZString getFieldTypeName(fieldtype type);

private:
    bool readPage(pageid page, ZBinary &data);
    bool loadHeadPage();

    pageid insertPage(pagetype type);
    bool writeHeadPage();

    bool zeroPad();

    bool freePage(pageid page);
    bool addToFreelist(pageid page);

    static zu16 getFieldFileId(fieldtype type);
    static fieldtype fieldFileIdToFieldType(zu16 type);

private:
    class ParcelPage : public ZWriter, public ZReader, public ZPosition {
    public:
        ParcelPage(ZFile *file, zu32 page, zu32 pagesize);

        zu64 read(zbyte *dest, zu64 size);
        zu64 write(const zbyte *src, zu64 size);

        zu64 setPos(zu64 pos){ _rwpos = pos; return _rwpos; }
        zu64 getPos() const { return _rwpos; }
        bool atEnd() const;

    protected:
        ZFile *_file;
        pageid _page;
        zu32 _pagesize;
        zu64 _rwpos;
    };

    class FieldPage : public ParcelPage {
    public:
        FieldPage(ZFile *file, zu32 page, zu32 pagesize);

        pagetype _pagetype;
        pageid _prevpage;
        pageid _nextpage;
    };

private:
    ZFile *_file;
    bool _init;
    zu32 _pagesize;
    zu32 _pagecount;

    zu8 _pagepower;
    zu32 _maxpages;
    pageid _freelistpage;
    pageid _fieldpage;
    pageid _indexpage;
    pageid _recordpage;

    ZMap<pageid, ZPointer<ParcelPage> > _pagecache;
};

}

/* ZParcel version 4 format
  A ZParcel 4 file is page-based
  Pages may be in any order, except the head page, which must be the first page
  The page size is the smallest I/O operation that may be performed at a time

  Head Page - defines parcel options
  48 bits: parcel signature
   8 bits: parcel category (1)
   8 bits: parcel version (4)
   8 bits: page size power (minimum 5 (32), default 10 (1K), maximum 32 (4G))
  32 bits: maximum number of pages (default 64K)
  32 bits: freelist page number (0 if none)
  32 bits: field page number
  32 bits: index page number
  32 bits: record page number
  .. zero padding ..


  Field Page - defines the usable fields in this file
   8 bits: page type
  32 bits: previous page number
  16 bits: number of fields
  16 bits: field id
   8 bits: field type
   8 bits: field name length (n)
   n bits: field name
  .. more fields ..
  32 bits: next page number


  Freelist Page - list of pages that are unused and not at the end of the file
   8 bits: page type
  32 bits: previous page number (0 if none)
  32 bits: free page number
  .. more free pages ..
  32 bits: next page number (0 if none)

  Free Page
   8 bits: page type
  .. zeroes ...


  Index Page - shortlist of record locations
   8 bits: page type
  32 bits: previous page number
  16 bits: field id
   n bits: field data
  32 bits: page number
  32 bits: page byte
  .. more record locations ..
  32 bits: next page number


  Record Page - contains records
   8 bits: page type
  32 bits: previous page number (0 if none)
  16 bits: number of fields in record
  16 bits: field id
   n bits: field data
  .. more fields ..
  .. more records ..
  32 bits: next page number (0 if none)


  Blob Page
   8 bits: page type
  32 bits: previous page number (0 if none)
   n bits: blob data
  32 bits: next page number (0 if none)


  History Page
   8 bits: page type


  Page Types
   0: free page
   1: field page
   2: freelist page
   3: index page
   4: record page
   5: blob page
   6: history page
  80: head page

  Field Types
   0:
   1:

*/

#endif // ZPARCEL4PARSER
