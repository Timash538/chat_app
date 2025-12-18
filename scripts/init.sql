CREATE TABLE Users (
    id SERIAL PRIMARY KEY,
    login TEXT UNIQUE NOT NULL,
    username TEXT NOT NULL,
    password_hash TEXT NOT NULL,
    is_deleted boolean NOT NULL DEFAULT false
);

CREATE TABLE Chats (
    id SERIAL PRIMARY KEY,
    type TEXT NOT NULL,
    name TEXT 
);

CREATE TABLE ChatMembers (
    chat_id INTEGER REFERENCES Chats(id),
    user_id INTEGER REFERENCES Users(id),
    PRIMARY KEY (chat_id, user_id)
);

CREATE TABLE Messages (
    id SERIAL PRIMARY KEY,
    chat_id INTEGER NOT NULL REFERENCES Chats(id),
    sender_id INTEGER NOT NULL REFERENCES Users(id),
    content TEXT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);