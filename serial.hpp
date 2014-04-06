
#ifndef DEF_SERIAL
#define DEF_SERIAL

class Serial
{
    public:
        Serial();
        bool open();
        bool opened() const;

        int read(char* buf, int size);
        void write(char byte);

    private:
        bool m_opened;
};

#endif

