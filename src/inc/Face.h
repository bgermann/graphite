/*  GRAPHITE2 LICENSING

    Copyright 2010, SIL International
    All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should also have received a copy of the GNU Lesser General Public
    License along with this library in the file named "LICENSE".
    If not, write to the Free Software Foundation, 51 Franklin Street, 
    Suite 500, Boston, MA 02110-1335, USA or visit their web page on the 
    internet at http://www.fsf.org/licenses/lgpl.html.

Alternatively, the contents of this file may be used under the terms of the
Mozilla Public License (http://mozilla.org/MPL) or the GNU General Public
License, as published by the Free Software Foundation, either version 2
of the License or (at your option) any later version.
*/
#pragma once

#include "graphite2/Font.h"

#include "inc/Main.h"
#include "inc/FeatureMap.h"
#include "inc/TtfUtil.h"
#include "inc/Silf.h"

namespace graphite2 {

class Cmap;
class FileFace;
class GlyphCache;
class NameTable;

using TtfUtil::Tag;

// These are the actual tags, as distinct from the consecutive IDs in TtfUtil.h

class Face
{
    static float default_glyph_advance(const void* face_ptr, gr_uint16 glyphid);

public:
	class Table;

    Face(const void* appFaceHandle/*non-NULL*/, const gr_face_ops & ops);
    virtual ~Face();

//    const Silf *silf(int i) const { return ((i < m_numSilf) ? m_silfs + i : (const Silf *)NULL); }
    virtual bool        runGraphite(Segment *seg, const Silf *silf) const;

public:
    bool                readGlyphs(uint32 faceOptions);
    bool                readGraphite();
    bool                readFeatures();
    void                takeFileFace(FileFace* pFileFace/*takes ownership*/);


    const SillMap     & theSill() const;
    const GlyphCache      & glyphs() const;
    Cmap              & cmap() const;
    NameTable         * nameTable() const;

    const Silf        * chooseSilf(uint32 script) const;
    uint16              languageForLocale(const char * locale) const;

    // Features
    uint16              numFeatures() const;
    const FeatureRef  * featureById(uint32 id) const;
    const FeatureRef  * feature(uint16 index) const;

    // Glyph related
    uint16 getGlyphMetric(uint16 gid, uint8 metric) const;
    uint16 findPseudo(uint32 uid) const;

    CLASS_NEW_DELETE;
private:
    const void* m_appFaceHandle/*non-NULL*/;
    gr_face_ops m_ops;
    uint16 m_ascent;
    uint16 m_descent;
    // unsigned short *m_glyphidx;     // index for each glyph id in the font
    // unsigned short m_readglyphs;    // how many glyphs have we in m_glyphs?
    // unsigned short m_capacity;      // how big is m_glyphs
    mutable GlyphCache* m_pGlyphFaceCache;      //owned - never NULL
    mutable Cmap * m_cmap; // cmap cache if available
protected:
    unsigned short m_numSilf;       // number of silf subtables in the silf table
    Silf *m_silfs;                   // silf subtables.
private:
    SillMap m_Sill;
    FileFace* m_pFileFace;      //owned
    mutable NameTable* m_pNames;
    
private:        //defensive on m_pGlyphFaceCache, m_pFileFace and m_silfs
    Face(const Face&);
    Face& operator=(const Face&);

    friend class Font;
};

inline
const SillMap & Face::theSill() const
{
    return m_Sill;
}

inline
uint16 Face::numFeatures() const
{
    return m_Sill.theFeatureMap().numFeats();
}

inline
const FeatureRef * Face::featureById(uint32 id) const
{
    return m_Sill.theFeatureMap().findFeatureRef(id);
}

inline
const FeatureRef *Face::feature(uint16 index) const
{
    return m_Sill.theFeatureMap().feature(index);
}

inline
const GlyphCache & Face::glyphs() const
{
	return *m_pGlyphFaceCache;
}

inline
Cmap & Face::cmap() const
{
    return *m_cmap;
};



class Face::Table
{
    const Face *            _f;
    mutable const byte *    _p;
    uint32                  _sz;

public:
    Table() throw();
    Table(const Face & face, const Tag n) throw();
    Table(const Table & rhs) throw();
    ~Table() throw();

    operator const byte * () const throw();

    Table & operator = (const Table & rhs) throw();
    size_t  size() const throw();
};

inline
Face::Table::Table() throw()
: _f(0), _p(0), _sz(0)
{
}

inline
Face::Table::Table(const Table & rhs) throw()
: _f(rhs._f), _p(rhs._p), _sz(rhs._sz)
{
	rhs._p = 0;
}

inline
Face::Table::~Table() throw()
{
	if (_p && _f->m_ops.release_table)
		(*_f->m_ops.release_table)(_f->m_appFaceHandle, _p);
}

inline
Face::Table::operator const byte * () const throw()
{
	return _p;
}

inline
size_t 	Face::Table::size() const throw()
{
	return _sz;
}

} // namespace graphite2

struct gr_face : public graphite2::Face {};
