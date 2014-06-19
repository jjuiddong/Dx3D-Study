
/*
 Using an output stream for debugging (http://www.codetools.com/useritems/debugout.asp)
 By Sven Axelsson (svenax@bredband.net)

 modified: 
 Konstantin Boukreev (konstantin@mail.primorye.ru)

 modified: 
 jjuiddong (jjuiddong@hanmail.net)
 
 Created: 10.15.2003
 Version: 1.0.0
*/



#if !defined(__DBGSTREAM_H__)
#define __DBGSTREAM_H__

#include <ostream>
#include <streambuf>
#include <string>



////////////////////////////////////////////////////////////////////////////////
/////////////// basic_outputbuf ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <typename tChar, typename tTraits = std::char_traits<tTraits> >
class basic_outputbuf : public std::basic_streambuf<tChar, tTraits>
{
protected:
    int_type overflow(int_type ch = traits_type::eof())
    {
        if (!traits_type::eq_int_type(traits_type::eof(), ch))
			m_buf += traits_type::to_char_type(ch);
        else
            return (int_type)sync();
        return traits_type::not_eof(ch);
    }
    int sync()
    {
        output_output_string(m_buf.c_str());
        m_buf.erase();
        return 0;
    }
	void output_output_string(const tChar *text) 
		{ ::OutputDebugStringA(text); }
private:
    std::basic_string<char_type, traits_type> m_buf;
};


////////////////////////////////////////////////////////////////////////////////
/////////////// basic_consolbuf ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <typename tChar, typename tTraits = std::char_traits<tTraits> >
class basic_consolbuf : public std::basic_streambuf<tChar, tTraits>
{
protected:
    int_type overflow(int_type ch = traits_type::eof())
    {
        if (!traits_type::eq_int_type(traits_type::eof(), ch))
			m_buf += traits_type::to_char_type(ch);
        else
            return (int_type)sync();
        return traits_type::not_eof(ch);
    }
    int sync()
    {
        output_consol_string(m_buf.c_str());
        m_buf.erase();
        return 0;
    }
	void output_consol_string(const tChar *text) 
	{ 
		DWORD dwWrite;
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		WriteFile(hOut, text, strlen(text), &dwWrite, NULL);
	}
private:
    std::basic_string<char_type, traits_type> m_buf;
};



////////////////////////////////////////////////////////////////////////////////
/////////////// basic_filelbuf /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <typename tChar, typename tTraits = std::char_traits<tTraits> >
class basic_filebuf1 : public std::basic_streambuf<tChar, tTraits>
{
public:
	basic_filebuf1( char *filename ) { strcpy_s(m_filename, sizeof(m_filename), filename); }
protected:
    int_type overflow(int_type ch = traits_type::eof())
    {
        if (!traits_type::eq_int_type(traits_type::eof(), ch))
			m_buf += traits_type::to_char_type(ch);
        else
            return (int_type)sync();
        return traits_type::not_eof(ch);
    }
    int sync()
    {
        output_file_string(m_buf.c_str());
        m_buf.erase();
        return 0;
    }
	void output_file_string(const tChar *text) 
	{
		FILE *fp;
		fopen_s( &fp, m_filename,"a+" );
		if( fp )
		{
			fprintf( fp,"%s", text );
			fclose( fp );
		}
	}
private:
	char m_filename[ 64];
    std::basic_string<char_type, traits_type> m_buf;
};



////////////////////////////////////////////////////////////////////////////////
/////////////// basic_messagebuf /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <typename tChar, typename tTraits = std::char_traits<tTraits> >
class basic_messagebuf : public std::basic_streambuf<tChar, tTraits>
{
protected:
    int_type overflow(int_type ch = traits_type::eof())
    {
        if (!traits_type::eq_int_type(traits_type::eof(), ch))
			m_buf += traits_type::to_char_type(ch);
        else
            return (int_type)sync();
        return traits_type::not_eof(ch);
    }
    int sync()
    {
        output_message_string(m_buf.c_str());
        m_buf.erase();
        return 0;
    }
	void output_message_string(const tChar *text) 
	{ ::MessageBox( NULL, text, "DBG_MESSAGE", MB_OK ); }
private:
    std::basic_string<char_type, traits_type> m_buf;
};



////////////////////////////////////////////////////////////////////////////////
//////////////basic_outputostream/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class tChar, class tTraits = std::char_traits<tChar> >
class basic_outputostream : public std::basic_ostream<tChar, tTraits>
{
private:
    basic_outputbuf<tChar, tTraits> m_debugbuf;
public:
    basic_outputostream() : std::basic_ostream<tChar, tTraits>(&m_debugbuf) {};
	std::basic_ostream<tChar, tTraits>& operator<< ( const tChar *ch )
	{
		m_debugbuf.sputn( ch, strlen(ch) );
		m_debugbuf.pubsync();
		return *this;
	}
};


////////////////////////////////////////////////////////////////////////////////
//////////////basic_consolostream////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class tChar, class tTraits = std::char_traits<tChar> >
class basic_consolostream : public std::basic_ostream<tChar, tTraits>
{
private:
    basic_consolbuf<tChar, tTraits> m_debugbuf;
public:
    basic_consolostream() : std::basic_ostream<tChar, tTraits>(&m_debugbuf) {};
	std::basic_ostream<tChar, tTraits>& operator<< ( const tChar *ch )
	{
		m_debugbuf.sputn( ch, strlen(ch) );
		m_debugbuf.pubsync();
		return *this;
	}
};


////////////////////////////////////////////////////////////////////////////////
//////////////basic_fileostream/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class tChar, class tTraits = std::char_traits<tChar> >
class basic_fileostream : public std::basic_ostream<tChar, tTraits>
{
private:
    basic_filebuf1<tChar, tTraits> m_debugbuf;
public:
    basic_fileostream( char *filename ) : std::basic_ostream<tChar, tTraits>(&m_debugbuf), m_debugbuf(filename ) {}
	std::basic_ostream<tChar, tTraits>& operator<< ( const tChar *ch )
	{
		m_debugbuf.sputn( ch, strlen(ch) );
		m_debugbuf.pubsync();
		return *this;
	}
};


////////////////////////////////////////////////////////////////////////////////
//////////////basic_messageostream/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class tChar, class tTraits = std::char_traits<tChar> >
class basic_messageostream : public std::basic_ostream<tChar, tTraits>
{
private:
    basic_messagebuf<tChar, tTraits> m_debugbuf;
public:
    basic_messageostream() : std::basic_ostream<tChar, tTraits>(&m_debugbuf) {}
	std::basic_ostream<tChar, tTraits>& operator<< ( const tChar *ch )
	{
		m_debugbuf.sputn( ch, strlen(ch) );
		m_debugbuf.pubsync();
		return *this;
	}
};

typedef basic_outputostream<char>	doutputostream;
typedef basic_consolostream<char>	dconsolostream;
typedef basic_fileostream<char>		dfileostream;
typedef basic_messageostream<char>	dmessageostream;


#endif // __DBGSTREAM_H__
