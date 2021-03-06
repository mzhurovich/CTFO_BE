/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2016 Dmitry "Dima" Korolev <dmitry.korolev@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

// This source file is just to generate golden data to fit the input converted entries to. -- D.K.

#include <cassert>
#include <fstream>

#include "../schema.h"
#include "../storage.h"

#include "../../Current/Bricks/dflags/dflags.h"

DEFINE_string(output, "db.json", "The name of the output DB file to generate.");

int main(int argc, char** argv) {
  ParseDFlags(&argc, &argv);

  using DB = CTFOStorage<SherlockStreamPersister>;
  DB db(FLAGS_output);

  current::storage::TransactionResult<bool> result =
      db.ReadOnlyTransaction([](ImmutableFields<DB> fields) -> bool {
        if (fields.user.Size() || fields.card.Size()) {
          std::cerr << "Storage loaded from `" << FLAGS_output << "` is not empty. Not adding anything.\n";
          return false;
        } else {
          return true;
        }
      }).Go();

  if (Value(result)) {
    db.ReadWriteTransaction([](MutableFields<DB> fields) {
      CTFO::User user;
      user.uid = static_cast<CTFO::UID>(42);
      user.level = 1;
      user.score = 9001;
      fields.user.Add(user);
    }).Go();

    db.ReadWriteTransaction([](MutableFields<DB> fields) {
      CTFO::Card card;
      card.cid = static_cast<CTFO::CID>(42);
      card.text = "В лесу родилась ёлочка";
      card.color.green = 0xff;
      card.tfu_count = 987654321;
      fields.card.Add(card);
    }).Go();

    db.ReadWriteTransaction([](MutableFields<DB> fields) {
      CTFO::Notification notification;
      CTFO::NotificationMyCardNewComment new_comment;
      notification.uid = static_cast<CTFO::UID>(42);
      notification.timestamp = std::chrono::microseconds(100000ull);
      notification.notification = new_comment;
      fields.notification.Add(notification);
    }).Go();
  }
}
