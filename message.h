#ifndef MESSAGE_H
#define MESSAGE_H

class QString;

/*
 * Nachrichtenklasse für das Observer-Modell
 */
class Message
{
public:
    enum MessageType
    {
        view_parse_text,
        view_representation_type_changed,
        view_copy_to_clipboard,
        parser_parser_error_message,
        parser_parsing_complete,
        parser_amount_instructions,
        parser_output
    };

    /*
     * In einer Union teilen sich alle Elemente
     * den selben Speicher, d.h. man kann nur eines
     * der Elemente verwenden.
     * Ist jedoch hier sehr praktisch um verschiedene Daten
     * in der selben Datenstruktur zu versenden.
     */
    union MessageData {
        void* pointer;
        QString* stringPointer;
        unsigned short us;
        short s;
        unsigned int ui;
        int i;
        bool b;
    };

    MessageType type;

    //Content of message
    MessageData data[16];

    Message();
};

#endif // MESSAGE_H
