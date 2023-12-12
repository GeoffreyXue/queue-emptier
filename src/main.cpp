// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.

#include <iostream>
#include <string>

#include <aws/core/Aws.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/crypto/Sha256.h>
#include <aws/sqs/SQSClient.h>
#include <aws/sqs/model/DeleteMessageRequest.h>
#include <aws/sqs/model/ReceiveMessageRequest.h>
#include <aws/sqs/model/SendMessageRequest.h>

using namespace std;

string kQueueUrlToEmpty =
    "https://sqs.us-east-2.amazonaws.com/848490464384/request.fifo";

string waitForResponse(const string &queueUrl);

int main() {
  Aws::SDKOptions options;
  Aws::InitAPI(options);
  {
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = Aws::Region::US_EAST_2; // Set the region to Ohio

    Aws::SQS::SQSClient sqs(clientConfig);

    // Create a receive message request
    Aws::SQS::Model::ReceiveMessageRequest receive_request;
    receive_request.SetQueueUrl(kQueueUrlToEmpty);
    receive_request.SetMaxNumberOfMessages(
        10); // Max number of messages to receive
    receive_request.SetVisibilityTimeout(30); // Visibility timeout
    receive_request.SetWaitTimeSeconds(20);   // Long polling wait time

    // Receive the message

    while (true) {
      auto receive_outcome = sqs.ReceiveMessage(receive_request);
      if (receive_outcome.IsSuccess()) {
        const auto &messages = receive_outcome.GetResult().GetMessages();
        if (!messages.empty()) {
          for (const auto &message : messages) {
            // Delete message from queue
            Aws::SQS::Model::DeleteMessageRequest delete_request;
            delete_request.SetQueueUrl(kQueueUrlToEmpty);
            delete_request.SetReceiptHandle(message.GetReceiptHandle());
            auto delete_outcome = sqs.DeleteMessage(delete_request);
            if (!delete_outcome.IsSuccess()) {
              std::cerr << "Error deleting message: "
                        << delete_outcome.GetError().GetMessage() << endl;
            }
          }
        } else {
          cout << "No messages to process." << endl;
          break;
        }
      } else {
        std::cerr << "Error receiving messages: "
                  << receive_outcome.GetError().GetMessage() << endl;
        break;
      }
    }
  }
  Aws::ShutdownAPI(options);

  return 0;
}
